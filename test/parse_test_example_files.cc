#include "gtest/gtest.h"

#include <string>
#include <string_view>
#include <filesystem>
#include "ecsact/parse.h"
#include "magic_enum.hpp"
#include "bazel_sundry/runfiles.hh"

namespace fs = std::filesystem;
using namespace std::string_literals;
using bazel_sundry::CreateDefaultRunfiles;

TEST(Parse, ExampleFiles) {
	auto runfiles = CreateDefaultRunfiles();
	ASSERT_TRUE(runfiles);

	std::vector<std::string> file_paths;
	ASSERT_FALSE(file_paths.emplace_back(
		runfiles->Rlocation("ecsact_parse/example/example.ecsact")
	).empty());
	ASSERT_FALSE(file_paths.emplace_back(
		runfiles->Rlocation("ecsact_parse/example/extras.ecsact")
	).empty());

	std::vector<const char*> file_cstr_paths;
	file_cstr_paths.reserve(file_paths.size());
	for(auto& p : file_paths) {
		ASSERT_TRUE(fs::exists(p));
		file_cstr_paths.push_back(p.c_str());
	}

	ecsact_parse_with_cpp_callback(
		file_cstr_paths.data(),
		file_cstr_paths.size(),
		[&](ecsact_parse_callback_params params) -> ecsact_parse_callback_result {
			EXPECT_FALSE(params.error);
			if(params.error) {
				std::cerr
					<< "\n\tParse Error: "
					<< magic_enum::enum_name(params.error->code) << "\n";
				std::cerr
					<< "\n\tFile: " << file_cstr_paths[params.source_file_index]
					<< "\n";
				if(params.error->error_location.length > 0) {
					auto err_loc = std::string_view(
						params.error->error_location.data,
						params.error->error_location.length
					);
					std::cerr << "\n\tRelevant source: " << err_loc << "\n";
				}
			} else {
				EXPECT_TRUE(params.statement);
				if(params.statement) {
					switch(params.statement->type) {
						// We should expect certain statements to always have a context
						case ECSACT_STATEMENT_BUILTIN_TYPE_FIELD:
						case ECSACT_STATEMENT_USER_TYPE_FIELD:
						case ECSACT_STATEMENT_ENUM_VALUE:
						case ECSACT_STATEMENT_SYSTEM_GENERATES:
							EXPECT_TRUE(params.context_statement)
								<< "Expected "
								<< magic_enum::enum_name(params.statement->type)
								<< " (" << params.statement->type << ")"
								<< " to have context statement";
							break;
						default:
							break;
					}
				}
			}

			return ECSACT_PARSE_CALLBACK_CONTINUE;
		}
	);
}
