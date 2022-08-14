#include "gtest/gtest.h"

#include <ecsact/parse.h>
#include <string_view>
#include <string>

using namespace std::string_literals;

void TestValidField
	( std::string_view       statement_str
	, ecsact_statement_type  expected_field_type
	, std::string_view       expected_field_name
	)
{
	auto component_name = "ExampleComponent"s;
	ecsact_statement component_statement{
		.type = ECSACT_STATEMENT_COMPONENT,
		.data{.component_statement{
			.component_name{
				.data = component_name.data(),
				.length = static_cast<int>(component_name.size()),
			}
		}},
	};

	ecsact_statement statement;
	ecsact_parse_status status;

	auto read_amount = ecsact_parse_statement(
		statement_str.data(),
		statement_str.size(),
		&component_statement,
		&statement,
		&status
	);

	ASSERT_EQ(statement.type, expected_field_type);
	EXPECT_EQ(status.code, ECSACT_PARSE_STATUS_OK);

	EXPECT_EQ(expected_field_name, std::string_view(
		statement.data.field_statement.field_name.data,
		statement.data.field_statement.field_name.length
	));

	EXPECT_EQ(read_amount, statement_str.size());
}

TEST(Parse, I8Field) {
	TestValidField(
		"i8 example_field;"s,
		ECSACT_STATEMENT_I8_FIELD,
		"example_field"
	);
}

TEST(Parse, U8Field) {
	TestValidField(
		"u8 example_field;"s,
		ECSACT_STATEMENT_U8_FIELD,
		"example_field"
	);
}

TEST(Parse, I16Field) {
	TestValidField(
		"i16 example_field;"s,
		ECSACT_STATEMENT_I16_FIELD,
		"example_field"
	);
}

TEST(Parse, U16Field) {
	TestValidField(
		"u16 example_field;"s,
		ECSACT_STATEMENT_U16_FIELD,
		"example_field"
	);
}

TEST(Parse, I32Field) {
	TestValidField(
		"i32 example_field;"s,
		ECSACT_STATEMENT_I32_FIELD,
		"example_field"
	);
}

TEST(Parse, U32Field) {
	TestValidField(
		"u32 example_field;"s,
		ECSACT_STATEMENT_U32_FIELD,
		"example_field"
	);
}

TEST(Parse, F32Field) {
	TestValidField(
		"f32 example_field;"s,
		ECSACT_STATEMENT_F32_FIELD,
		"example_field"
	);
}

TEST(Parse, EntityField) {
	TestValidField(
		"entity example_field;"s,
		ECSACT_STATEMENT_ENTITY_FIELD,
		"example_field"
	);
}
