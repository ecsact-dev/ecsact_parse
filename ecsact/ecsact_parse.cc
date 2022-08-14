#include "parse.h"

#include <fstream>

#include "./detail/check_set.hh"
#include "./detail/fixed_stack.hh"
#include "./detail/read_util.hh"
#include "./detail/stack_util.hh"

using ecsact::parse::detail::fixed_stack;
using ecsact::parse::detail::check_set;
using ecsact::parse::detail::try_top;

struct statement_and_source {
	ecsact_statement statement;
	std::string source;
};

struct file_and_state {
	std::ifstream stream;
	statement_and_source package;
	fixed_stack<statement_and_source, 16> statement_stack;
	std::vector<std::string> next_statement_sources;

	std::vector<std::string> imports;
	int32_t last_statement_id = 0;

	bool can_read_next() const {
		return !next_statement_sources.empty() || (stream && !stream.eof());
	}

	void read_next_statement(ecsact_parse_status& status) {
		using ecsact::parse::detail::stream_get_until;
		auto previous = try_top(statement_stack);
		auto& next = statement_stack.emplace();

		if(next_statement_sources.empty()) {
			next.source.reserve(240);
			stream_get_until(stream, next.source, std::array{';', '{', '}'});
			next.source.shrink_to_fit();
		} else {
			next.source = next_statement_sources.back();
			next_statement_sources.pop_back();
		}

		auto read_data = next.source.data();
		auto read_size = next.source.size();

		[[maybe_unused]]
		auto read_amount = ecsact_parse_statement(
			read_data,
			read_size,
			previous ? &previous->get().statement : nullptr,
			&next.statement,
			&status
		);

		next.statement.id = ++last_statement_id;

		if(next.statement.type == ECSACT_STATEMENT_IMPORT) {
			imports.push_back(std::string(
				next.statement.data.import_statement.import_package_name.data,
				next.statement.data.import_statement.import_package_name.length
			));
		}

		if(!ecsact_is_error_parse_status_code(status.code)) {
			assert(read_amount == next.source.size());
		}
	}

	void pump_status_code(ecsact_parse_status_code status_code) {
		if(status_code == ECSACT_PARSE_STATUS_OK) {
			// We've reached the end of the current statement. Pop it off the stack.
			statement_stack.pop();
		} else if(status_code == ECSACT_PARSE_STATUS_BLOCK_END) {
			// We've reached the end of the current statement and the current block.
			// Pop the current and pop the block.
			statement_stack.pop();
			statement_stack.pop();
		}
	}

	void pop_rewind() {
		auto& last = statement_stack.top();
		next_statement_sources.push_back(last.source);
		statement_stack.pop();
	}
};

struct ecsact_parse_object {
	const char** file_paths;
	int file_paths_count;
	std::vector<file_and_state> files;
	ecsact_parse_status status;
	ecsact_parse_callback callback;
	void* callback_user_data;
	ecsact_parse_callback_params callback_params;
	ecsact_parse_error parse_error;
	int32_t next_id = 0;

	ecsact_parse_object
		( const char**           file_paths
		, int                    file_paths_count
		, ecsact_parse_callback  callback
		, void*                  callback_user_data
		)
		: file_paths(file_paths)
		, file_paths_count(file_paths_count)
		, callback(callback)
		, callback_user_data(callback_user_data)
	{
		std::vector<file_and_state> files;
		files.reserve(file_paths_count);
	}

	bool callback_recover() {
		switch(callback(callback_params, callback_user_data)) {
			case ECSACT_PARSE_CALLBACK_CONTINUE:
				return true;
			default:
				return false;
		}
	}

	bool callback_recover_if_error_status() {
		switch(status.code) {
			case ECSACT_PARSE_STATUS_OK:
			case ECSACT_PARSE_STATUS_BLOCK_BEGIN:
			case ECSACT_PARSE_STATUS_BLOCK_END:
				return true;
			default:
				callback_params.context_statement = nullptr;
				callback_params.package = nullptr;
				callback_params.statement = nullptr;
				callback_params.error = &parse_error;
				parse_error.code = ECSACT_PARSE_ERR_STATUS;
				parse_error.error_location = status.error_location;
				return callback_recover();
		}
	}

	bool parse_packages_top() {
		for(int i=0; file_paths_count > i; ++i) {
			auto& file = files.emplace_back();
			callback_params.source_file_index = i;

			file.stream.open(file_paths[i], std::ios_base::in | std::ios_base::binary);
			file.read_next_statement(status);

			if(!callback_recover_if_error_status()) {
				return false;
			}

			auto& statement = file.statement_stack.top().statement;
			file.package.statement.type = ECSACT_STATEMENT_PACKAGE;
			auto& package_data = file.package.statement.data.package_statement;

			if(statement.type != ECSACT_STATEMENT_PACKAGE) {
				callback_params.context_statement = nullptr;
				callback_params.package = nullptr;
				callback_params.statement = &statement;
				callback_params.error = &parse_error;
				parse_error.code = ECSACT_PARSE_ERR_EXPECTED_PACKAGE_STATEMENT;
				parse_error.error_location = {};
				if(!callback_recover()) {
					return false;
				}
				file.package.source = "";
				package_data.main = false;
				package_data.package_name = {
					.data = file.package.source.data(),
					.length = 0,
				};
				file.pop_rewind();
			} else {

				file.package.statement.type = ECSACT_STATEMENT_PACKAGE;
				file.package.statement.data.package_statement = {
					.main = statement.data.package_statement.main,
					.package_name = {},
				};

				// Copy the original source so we can do a 'mock' copy to get the correct
				// offset on the string view.
				file.package.source = file.statement_stack.top().source;
				ecsact_statement_sv_mock_copy(
					file.statement_stack.top().source.data(),
					&statement.data.package_statement.package_name,
					file.package.source.data(),
					&file.package.statement.data.package_statement.package_name
				);

				callback_params.context_statement = nullptr;
				callback_params.package = nullptr;
				callback_params.statement = &statement;
				callback_params.error = nullptr;
				if(!callback_recover()) {
					return false;
				}
				file.pump_status_code(status.code);
			}
		}

		return true;
	}

	signed get_package_file_index(const std::string& package_name) {
		for(signed i=0; file_paths_count > i; ++i) {
			auto& file = files[i];
			std::string_view file_pkg_name(
				file.package.statement.data.package_statement.package_name.data,
				file.package.statement.data.package_statement.package_name.length
			);

			if(file_pkg_name == package_name) {
				return i;
			}
		}

		return -1;
	}

	bool parse_imports() {
		for(int i=0; file_paths_count > i; ++i) {
			auto& file = files[i];
			callback_params.package = &file.package.statement.data.package_statement;
			callback_params.source_file_index = i;

			while(file.can_read_next()) {
				file.read_next_statement(status);

				if(!callback_recover_if_error_status()) {
					return false;
				}

				auto& statement = file.statement_stack.top().statement;

				if(statement.type != ECSACT_STATEMENT_IMPORT) {
					file.pop_rewind();
					break;
				}

				callback_params.context_statement = nullptr;
				callback_params.statement = &statement;
				callback_params.error = nullptr;
				if(!callback_recover()) {
					return false;
				}

				file.pump_status_code(status.code);
			}
		}

		return true;
	}
	
	bool check_unknown_imports() {
		for(int i=0; file_paths_count > i; ++i) {
			auto& file = files[i];
			for(auto& imp : file.imports) {
				if(get_package_file_index(imp) == -1) {
					return false;
				}
			}
		}

		return true;
	}

	bool check_cyclic_imports(int start_index, int current_index) {
		if(start_index == current_index) {
			return false;
		}

		auto& file = files[current_index];
		for(auto& imp : file.imports) {
			if(!check_cyclic_imports(start_index, get_package_file_index(imp))) {
				return false;
			}
		}

		return true;
	}

	bool check_cyclic_imports(int index) {
		auto& file = files[index];
		for(auto& imp : file.imports) {
			if(!check_cyclic_imports(index, get_package_file_index(imp))) {
				return false;
			}
		}

		return true;
	}

	bool check_cyclic_imports() {
		for(int i=0; file_paths_count > i; ++i) {
			if(!check_cyclic_imports(i)) {
				return false;
			}
		}

		return true;
	}

	bool parse_declarations() {
		check_set parsed_check;
		parsed_check.resize(files.size());

		auto all_imports_parsed = [&](file_and_state& file) -> bool {
			for(auto& imp : file.imports) {
				auto index = get_package_file_index(imp);
				if(!parsed_check.get(index)) {
					return false;
				}
			}

			return true;
		};

		while(!parsed_check.complete()) {
			for(int i=0; file_paths_count > i; ++i) {
				// Skip already parsed
				if(parsed_check.get(i)) {
					continue;
				}

				auto& file = files[i];
				if(!all_imports_parsed(file)) {
					continue;
				}

				callback_params.package = &file.package.statement.data.package_statement;
				callback_params.source_file_index = i;

				while(file.can_read_next()) {
					file.read_next_statement(status);

					if(!callback_recover_if_error_status()) {
						return false;
					}

					auto& statement = file.statement_stack.top().statement;

					callback_params.context_statement = nullptr;
					callback_params.statement = &statement;
					callback_params.error = nullptr;
					if(!callback_recover()) {
						return false;
					}

					file.pump_status_code(status.code);
				}

				parsed_check.check(i);
			}
		}

		return true;
	}
};

void ecsact_parse
	( const char**           file_paths
	, int                    file_paths_count
	, ecsact_parse_callback  callback
	, void*                  callback_user_data
	)
{
	ecsact_parse_object parse_obj(
		file_paths,
		file_paths_count,
		callback,
		callback_user_data
	);

	// 1. Parse the tops of each file for their package statement
	if(!parse_obj.parse_packages_top())     return;
	// 2. Parse the top of each file for each import statement
	if(!parse_obj.parse_imports())          return;
	if(!parse_obj.check_unknown_imports())  return;
	if(!parse_obj.check_cyclic_imports())   return;
	// 3. se the rest for all declarations
	if(!parse_obj.parse_declarations())     return;

	// Done!
}
