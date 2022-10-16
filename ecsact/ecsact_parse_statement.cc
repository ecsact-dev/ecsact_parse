#include "parse.h"

#include <lexy/input/string_input.hpp>
#include <lexy/input_location.hpp>
#include <lexy/action/scan.hpp>

#include "./detail/grammar.hh"

using namespace ecsact::parse::detail;

template<typename Fn>
static auto context_grammar
	( ecsact_statement_type  context_type
	, Fn&&                   fn
	)
{
	switch(context_type) {
		case ECSACT_STATEMENT_NONE:
			return fn(grammar::top_level_statement{});
		case ECSACT_STATEMENT_UNKNOWN:
			return fn(grammar::unknown_level_statement{});
		case ECSACT_STATEMENT_ENUM:
			return fn(grammar::enum_level_statements{});
		case ECSACT_STATEMENT_COMPONENT:
			return fn(grammar::component_level_statement{});
		case ECSACT_STATEMENT_TRANSIENT:
			return fn(grammar::transient_level_statement{});
		case ECSACT_STATEMENT_SYSTEM:
			return fn(grammar::system_level_statement{});
		case ECSACT_STATEMENT_ACTION:
			return fn(grammar::action_level_statement{});
		case ECSACT_STATEMENT_SYSTEM_GENERATES:
			return fn(grammar::system_generates_level_statement{});
		case ECSACT_STATEMENT_ENTITY_FIELD:
			return fn(grammar::entity_field_level_statement{});
		case ECSACT_STATEMENT_SYSTEM_COMPONENT:
			return fn(grammar::system_component_level_statement{});
		case ECSACT_STATEMENT_SYSTEM_WITH_ENTITY:
			return fn(grammar::system_with_entity_level_statement{});
	}

	assert(false && "unhandled context grammar");
	return fn(grammar::top_level_statement{});
}

int ecsact_parse_statement
	( const char*              statement_cstr
	, int                      max_read_length
	, const ecsact_statement*  context_statement
	, ecsact_statement*        out_statement
	, ecsact_parse_status*     out_status
	)
{
	static decltype(out_statement->id) last_id{};

	const auto context_type = context_statement == nullptr
		? ECSACT_STATEMENT_NONE
		: context_statement->type;

	auto input = lexy::string_input(statement_cstr, max_read_length);
	auto scanner = lexy::scan<grammar::control>(
		input,
		lexy::callback<void>([&](auto ctx, auto err) {
			out_status->code = ECSACT_PARSE_STATUS_SYNTAX_ERROR;
			out_status->error_location = {
				.data = ctx.input().data(),
				.length = static_cast<int>(ctx.input().size()),
			};
		})
	);

	lexy::scan_result<grammar::statement_value> result = context_grammar(
		context_type,
		[&]<typename Grammar>(Grammar) { return scanner.parse<Grammar>(); }
	);

	if(result) {
		*out_statement = result.value().statement;
		out_status->code = result.value().status;
	}

	out_statement->id = ++last_id;

	auto remaining_input = scanner.remaining_input();
	return remaining_input.reader().position() - input.data();
}
