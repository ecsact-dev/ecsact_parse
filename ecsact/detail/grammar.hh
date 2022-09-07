#pragma once

#include <string_view>
#include <optional>
#include <lexy/dsl.hpp>
#include <lexy/callback.hpp>
#include "ecsact/parse/status.h"
#include "ecsact/parse/statements.h"
#include "magic_enum.hpp"

namespace ecsact::parse::detail::grammar {
	struct statement_end {
		static constexpr auto rule = lexy::dsl::lit_c<';'>;
		static constexpr auto value = lexy::constant(ECSACT_PARSE_STATUS_OK);
	};

	struct statement_block_begin {
		static constexpr auto rule = lexy::dsl::lit_c<'{'>;
		static constexpr auto value =
			lexy::constant(ECSACT_PARSE_STATUS_BLOCK_BEGIN);
	};

	struct statement_block_end {
		static constexpr auto rule = lexy::dsl::lit_c<'}'>;
		static constexpr auto value = lexy::constant(ECSACT_PARSE_STATUS_BLOCK_END);
	};

	struct eof {
		static constexpr auto rule = lexy::dsl::eof;
		static constexpr auto value = lexy::constant(ECSACT_PARSE_STATUS_OK);
	};

	struct parse_end {
		static constexpr auto rule =
			lexy::dsl::p<statement_end> |
			lexy::dsl::p<statement_block_end> |
			lexy::dsl::p<statement_block_begin> |
			lexy::dsl::p<eof>;
		static constexpr auto value = lexy::forward<ecsact_parse_status_code>;
	};

	struct statement_body {
		static constexpr auto rule = lexy::dsl::identifier(
			lexy::dsl::ascii::character - (
				lexy::dsl::inline_<statement_end> /
				lexy::dsl::inline_<statement_block_begin> /
				lexy::dsl::inline_<statement_block_end>
			)
		);
		static constexpr auto value = lexy::as_string<std::string_view>;
	};

	struct line_comment {
		static constexpr auto rule =
			LEXY_LIT("//") >> lexy::dsl::until(lexy::dsl::newline);
	};

	struct block_comment {
		static constexpr auto rule =
			LEXY_LIT("/*") >> lexy::dsl::until(LEXY_LIT("*/"));
	};

	struct whitespace {
		static constexpr auto rule =
			lexy::dsl::ascii::space |
			lexy::dsl::inline_<line_comment> |
			lexy::dsl::inline_<block_comment>;
	};

	struct type_name {
		static constexpr auto rule = lexy::dsl::identifier(
			lexy::dsl::ascii::alpha,
			lexy::dsl::ascii::alnum
		);

		static constexpr auto value = lexy::as_string<std::string_view>;
	};

	struct package_name {
		static constexpr auto rule = lexy::dsl::identifier(
			lexy::dsl::ascii::alpha,
			lexy::dsl::ascii::alnum / lexy::dsl::lit_c<'.'> / lexy::dsl::lit_c<'_'>
		);

		static constexpr auto value = lexy::as_string<std::string_view>;
	};

	struct field_name {
		static constexpr auto rule = lexy::dsl::identifier(
			lexy::dsl::ascii::alpha,
			lexy::dsl::ascii::alnum / lexy::dsl::lit_c<'_'>
		);

		static constexpr auto value = lexy::as_string<std::string_view>;
	};

	struct package_statement {
		static constexpr auto name() { return "import statement"; }

		struct main_package_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("main") >> LEXY_LIT("package");
			static constexpr auto value = lexy::constant(true);
		};

		struct package_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("package");
			static constexpr auto value = lexy::constant(false);
		};

		static constexpr auto rule =
			(lexy::dsl::p<main_package_keyword> | lexy::dsl::p<package_keyword>) >>
			lexy::dsl::p<package_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](bool main, std::string_view package_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_PACKAGE,
					.data{.package_statement{
						.main = main,
						.package_name{
							.data = package_name.data(),
							.length = static_cast<int>(package_name.size()),
						},
					}},
				};
			}
		);
	};

	struct import_statement {
		static constexpr auto name() { return "import statement"; }

		struct import_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("import");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<import_keyword> >>
			lexy::dsl::p<package_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view import_package_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_IMPORT,
					.data{.import_statement{
						.import_package_name{
							.data = import_package_name.data(),
							.length = static_cast<int>(import_package_name.size()),
						},
					}},
				};
			}
		);
	};

	struct component_statement {
		static constexpr auto name() { return "component statement"; }

		struct component_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("component");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<component_keyword> >>
			lexy::dsl::p<type_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view component_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_COMPONENT,
					.data{.component_statement{
						.component_name{
							.data = component_name.data(),
							.length = static_cast<int>(component_name.size()),
						},
					}},
				};
			}
		);
	};

struct transient_statement {
		static constexpr auto name() { return "transient statement"; }

		struct transient_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("transient");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<transient_keyword> >>
			lexy::dsl::p<type_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view transient_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_TRANSIENT,
					.data{.transient_statement{
						.transient_name{
							.data = transient_name.data(),
							.length = static_cast<int>(transient_name.size()),
						},
					}},
				};
			}
		);
	};

	struct system_statement {
		struct system_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("system");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<system_keyword> >>
			lexy::dsl::opt(lexy::dsl::p<type_name>);

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::optional<std::string_view> system_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_SYSTEM,
					.data{.system_statement{
						.system_name{
							.data = system_name ? system_name->data() : nullptr,
							.length = system_name ? static_cast<int>(system_name->size()) : 0,
						},
					}},
				};
			}
		);
	};

	struct action_statement {
		static constexpr auto name() { return "action statement"; }

		struct action_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("action");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<action_keyword> >>
			lexy::dsl::p<type_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view action_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_ACTION,
					.data{.action_statement{
						.action_name{
							.data = action_name.data(),
							.length = static_cast<int>(action_name.size()),
						},
					}},
				};
			}
		);
	};

	struct enum_statement {
		static constexpr auto name() { return "enum statement"; }

		struct enum_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("enum");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<enum_keyword> >>
			lexy::dsl::p<type_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view enum_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_ENUM,
					.data{.enum_statement{
						.enum_name{
							.data = enum_name.data(),
							.length = static_cast<int>(enum_name.size()),
						},
					}},
				};
			}
		);
	};

	struct enum_value_statement {
		static constexpr auto name() { return "enum value statement"; }

		struct enum_value_integer {
			static constexpr auto rule = [] {
				using namespace lexy;
				auto digits = dsl::digits<>.sep(dsl::digit_sep_tick).no_leading_zero();
				return dsl::integer<int32_t>(digits);
			}();

			static constexpr auto value = lexy::as_integer<int32_t>;
		};

		static constexpr auto rule = [] {
			using lexy::dsl::p;
			using lexy::dsl::lit_c;

			return p<field_name> >> lit_c<'='> >> p<enum_value_integer>;
		}();

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view name, int32_t value) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_ENUM_VALUE,
					.data{.enum_value_statement{
						.name{
							.data = name.data(),
							.length = static_cast<int>(name.size()),
						},
						.value = value,
					}},
				};
			}
		);
	};

	struct array_length {
		static constexpr auto name() { return "array length"; }

		struct integer {
			static constexpr auto rule = [] {
				using namespace lexy;
				auto digits = dsl::digits<>.sep(dsl::digit_sep_tick).no_leading_zero();
				return dsl::integer<int32_t>(digits);
			}();

			static constexpr auto value = lexy::as_integer<int32_t>;
		};

		static constexpr auto rule = [] {
			using lexy::dsl::lit_c;
			using lexy::dsl::p;

			return lit_c<'['> >> p<integer> >> lit_c<']'>;
		}();

		static constexpr auto value = lexy::forward<int32_t>;
	};

	struct field_statement {
		static constexpr auto name() { return "field statement"; }

		struct i8_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("i8");
			static constexpr auto value = lexy::constant(ECSACT_I8);
		};
		struct u8_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("u8");
			static constexpr auto value = lexy::constant(ECSACT_U8);
		};
		struct i16_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("i16");
			static constexpr auto value = lexy::constant(ECSACT_I16);
		};
		struct u16_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("u16");
			static constexpr auto value = lexy::constant(ECSACT_U16);
		};
		struct i32_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("i32");
			static constexpr auto value = lexy::constant(ECSACT_I32);
		};
		struct u32_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("u32");
			static constexpr auto value = lexy::constant(ECSACT_U32);
		};
		struct f32_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("f32");
			static constexpr auto value = lexy::constant(ECSACT_F32);
		};
		struct entity_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("entity");
			static constexpr auto value = lexy::constant(ECSACT_ENTITY_TYPE);
		};

		static constexpr auto type_keyword = (
			lexy::dsl::p<i8_keyword> |
			lexy::dsl::p<u8_keyword> |
			lexy::dsl::p<i16_keyword> |
			lexy::dsl::p<u16_keyword> |
			lexy::dsl::p<i32_keyword> |
			lexy::dsl::p<u32_keyword> |
			lexy::dsl::p<f32_keyword> |
			lexy::dsl::p<entity_keyword>
		);

		static constexpr auto rule =
			type_keyword >>
			lexy::dsl::opt(lexy::dsl::p<array_length>) >>
			lexy::dsl::p<field_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[]
				( ecsact_builtin_type   field_type
				, std::optional<int32_t>  length
				, std::string_view        field_name
				)
			{
				return ecsact_statement{
					.type = ECSACT_STATEMENT_BUILTIN_TYPE_FIELD,
					.data{.field_statement{
						.field_type = field_type,
						.field_name{
							.data = field_name.data(),
							.length = static_cast<int>(field_name.size()),
						},
						.length = length.value_or(1),
					}},
				};
			}
		);
	};

	struct user_type_field_statement {
		static constexpr auto name() { return "user type field statement"; }

		static constexpr auto rule =
			lexy::dsl::p<type_name> >>
			lexy::dsl::opt(lexy::dsl::p<array_length>) >>
			lexy::dsl::p<field_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[]
				( std::string_view        user_type_name
				, std::optional<int32_t>  length
				, std::string_view        field_name
				)
			{
				return ecsact_statement{
					.type = ECSACT_STATEMENT_USER_TYPE_FIELD,
					.data{.user_type_field_statement{
						.user_type_name{
							.data = user_type_name.data(),
							.length = static_cast<int>(user_type_name.size()),
						},
						.field_name{
							.data = field_name.data(),
							.length = static_cast<int>(field_name.size()),
						},
						.length = length.value_or(1),
					}},
				};
			}
		);
	};

	struct system_component_statement {
		static constexpr auto name() { return "system component statement"; }

		struct readonly_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("readonly");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_READONLY);
		};
		struct writeonly_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("writeonly");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_WRITEONLY);
		};
		struct readwrite_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("readwrite");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_READWRITE);
		};
		struct optional_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("optional");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_OPTIONAL);
		};
		struct include_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("include");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_INCLUDE);
		};
		struct exclude_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("exclude");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_EXCLUDE);
		};
		struct adds_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("adds");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_ADDS);
		};
		struct removes_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("removes");
			static constexpr auto value = lexy::constant(ECSACT_SYS_CAP_REMOVES);
		};

		struct filter {
			static constexpr auto rule =
				lexy::dsl::p<include_keyword> |
				lexy::dsl::p<exclude_keyword>;
			static constexpr auto value = lexy::forward<ecsact_system_capability>;
		};

		struct access {
			static constexpr auto primary = lexy::dsl::p<optional_keyword>;
			static constexpr auto secondary = (
				lexy::dsl::p<readonly_keyword> |
				lexy::dsl::p<writeonly_keyword> |
				lexy::dsl::p<readwrite_keyword>
			);

			static constexpr auto rule = (
				(primary >> secondary) |
				secondary
			);
			static constexpr auto value = lexy::callback<ecsact_system_capability>(
				[](ecsact_system_capability opt_cap, ecsact_system_capability cap) {
					using namespace magic_enum::bitwise_operators;
					return opt_cap | cap;
				},
				[](ecsact_system_capability cap) {
					return cap;
				}
			);
		};

		struct assignment {
			static constexpr auto rule =
				lexy::dsl::p<adds_keyword> |
				lexy::dsl::p<removes_keyword>;
			static constexpr auto value = lexy::forward<ecsact_system_capability>;
		};
		static_assert(!lexy::is_branch_rule<assignment>);

		struct unknown_keyword {
			static constexpr auto name() {
				return "unknown capability keyword";
			}
		};

		struct with_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("with");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto capability =
			lexy::dsl::p<filter> |
			lexy::dsl::p<access> |
			lexy::dsl::p<assignment>;

		static constexpr auto rule =
			capability >> lexy::dsl::p<type_name> >>
			lexy::dsl::opt(lexy::dsl::p<with_keyword> >> lexy::dsl::p<field_name>);
			
		static constexpr auto value = lexy::callback<ecsact_statement>(
			[]
				( ecsact_system_capability         cap
				, std::string_view                 component_name
				, std::optional<std::string_view>  with_entity_field_name
				)
			{
				ecsact_statement statement{
					.type = ECSACT_STATEMENT_SYSTEM_COMPONENT,
					.data = {},
				};

				auto& data = statement.data.system_component_statement;

				data.capability = cap;
				data.component_name.data = component_name.data();
				data.component_name.length = static_cast<int>(component_name.size());

				if(with_entity_field_name) {
					data.with_entity_field_name.data = with_entity_field_name->data();
					data.with_entity_field_name.length = static_cast<int>(
						with_entity_field_name->size()
					);
				}

				return statement;
			}
		);
	};

	struct generates_statement {
		struct generates_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("generates");
			static constexpr auto value = lexy::noop;
		};
		static constexpr auto rule = lexy::dsl::p<generates_keyword>;
		static constexpr auto value = lexy::callback<ecsact_statement>(
			[]() {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_SYSTEM_GENERATES,
					.data{},
				};
			}
		);
	};

	struct entity_constraint_statement {
		struct optional_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("optional");
			static constexpr auto value = lexy::constant(true);
		};
		struct required_keyword : lexy::token_production {
			static constexpr auto rule = LEXY_LIT("required");
			static constexpr auto value = lexy::constant(false);
		};

		static constexpr auto rule =
			(lexy::dsl::p<optional_keyword> | lexy::dsl::p<required_keyword>) >>
			lexy::dsl::p<type_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](bool optional, std::string_view component_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_ENTITY_CONSTRAINT,
					.data{.entity_constraint_statement{
						.optional = optional,
						.constraint_component_name{
							.data = component_name.data(),
							.length = static_cast<int>(component_name.size()),
						},
					}},
				};
			}
		);
	};

	struct with_statement {
		static constexpr auto name() { return "with statement"; }

		struct with_keyword : lexy::transparent_production {
			static constexpr auto rule = LEXY_LIT("with");
			static constexpr auto value = lexy::noop;
		};

		static constexpr auto rule =
			lexy::dsl::p<with_keyword> >> lexy::dsl::p<field_name>;

		static constexpr auto value = lexy::callback<ecsact_statement>(
			[](std::string_view field_name) {
				return ecsact_statement{
					.type = ECSACT_STATEMENT_SYSTEM_WITH_ENTITY,
					.data{.system_with_entity_statement{
						.with_entity_field_name{
							.data = field_name.data(),
							.length = static_cast<int>(field_name.size()),
						},
					}},
				};
			}
		);
	};

	struct none_statement {
		static constexpr auto rule = LEXY_LIT("");
		static constexpr auto value = lexy::constant(ecsact_statement{
			.type = ECSACT_STATEMENT_NONE,
			.data = {},
		});
	};

	// Simple control grammar used in lexy::scan
	struct control {
		static constexpr auto whitespace =
			lexy::dsl::inline_<ecsact::parse::detail::grammar::whitespace>;
	};

	struct statement_value {
		ecsact_statement statement;
		ecsact_parse_status_code status;
	};

	template<
		const char* Name,
		const char* ExpectedStatementErrorMessage,
		typename... Grammar>
	struct statement {
		static constexpr auto name() {
			return Name;
		}

		struct expected_statement {
			static constexpr auto name() {
				return ExpectedStatementErrorMessage;
			}
		};

		static constexpr auto whitespace =
			lexy::dsl::inline_<ecsact::parse::detail::grammar::whitespace>;

		static constexpr auto rule = [] {
			auto statement_p =
				(lexyd::p<Grammar> | ... ) |
				lexyd::p<none_statement> |
				lexyd::error<expected_statement>;

			auto statement = (lexyd::member<&statement_value::statement> = statement_p);
			auto status = (lexyd::member<&statement_value::status> = lexyd::p<parse_end>);

			return statement + status;
		}();

		static constexpr auto value = lexy::as_aggregate<statement_value>;
	};

	constexpr char top_level_statement_name[] = "top level statement";
	constexpr char top_level_statement_expected_message[] = "expected top level statement";
	using top_level_statement = statement
		< top_level_statement_name
		, top_level_statement_expected_message
		, package_statement
		, import_statement
		, component_statement
		, transient_statement
		, system_statement
		, action_statement
		, enum_statement
		>;

	constexpr char enum_level_statements_name[] = "enum value statements";
	constexpr char enum_level_statements_name_expected_message[] = "expected enum values";
	using enum_level_statements = statement
		< enum_level_statements_name
		, enum_level_statements_name_expected_message
		, enum_value_statement
		>;

	constexpr char component_level_statement_name[] = "component level statement";
	constexpr char component_level_statement_expected_message[] = "expected component level statement";
	using component_level_statement = statement
		< component_level_statement_name
		, component_level_statement_expected_message
		, field_statement
		, user_type_field_statement
		>;

	constexpr char transient_level_statement_name[] = "transient level statement";
	constexpr char transient_level_statement_expected_message[] = "expected transient level statement";
	using transient_level_statement = statement
		< transient_level_statement_name
		, transient_level_statement_expected_message
		, field_statement
		, user_type_field_statement
		>;

	constexpr char system_level_statement_name[] = "system level statement";
	constexpr char system_level_statement_expected_message[] = "expected system level statement";
	using system_level_statement = statement
		< system_level_statement_name
		, system_level_statement_expected_message
		, system_component_statement
		, generates_statement
		, system_statement
		>;

	constexpr char action_level_statement_name[] = "action level statement";
	constexpr char action_level_statement_expected_message[] = "expected action level statement";
	using action_level_statement = statement
		< action_level_statement_name
		, action_level_statement_expected_message
		, field_statement
		, system_component_statement
		, generates_statement
		, system_statement
		, user_type_field_statement
		>;

	constexpr char system_generates_level_statement_name[] = "system generates level statement";
	constexpr char system_generates_level_statement_expected_message[] = "expected system generates level statement";
	using system_generates_level_statement = statement
		< system_generates_level_statement_name
		, system_generates_level_statement_expected_message
		, entity_constraint_statement
		>;

	constexpr char entity_field_level_statement_name[] = "entity field level statement";
	constexpr char entity_field_level_statement_expected_message[] = "expected entity field level statement";
	using entity_field_level_statement = statement
		< entity_field_level_statement_name
		, entity_field_level_statement_expected_message
		, entity_constraint_statement
		>;

	constexpr char system_component_level_statement_name[] = "system component level statement";
	constexpr char system_component_level_statement_expected_message[] = "expected system component level statement";
	using system_component_level_statement = statement
		< system_component_level_statement_name
		, system_component_level_statement_expected_message
		, system_component_statement
		, with_statement
		>;
}
