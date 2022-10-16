#include "gtest/gtest.h"

#include <string_view>
#include <string>
#include "ecsact/parse.h"

using namespace std::string_literals;

TEST(Parse, WithEntityStatement) {
	ecsact_statement context;
	context.type = ECSACT_STATEMENT_SYSTEM_COMPONENT;
	context.data.system_component_statement = {
		.component_name = {},
		.with_entity_field_name = {},
	};

	auto statement_str = "with test_entity;"s;
	ecsact_statement statement;
	ecsact_parse_status status;

	auto read_amount = ecsact_parse_statement(
		statement_str.data(),
		statement_str.size(),
		&context,
		&statement,
		&status
	);

	ASSERT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM_WITH_ENTITY);
	EXPECT_EQ(status.code, ECSACT_PARSE_STATUS_OK);

	EXPECT_EQ("test_entity", std::string_view(
		statement.data.system_with_entity_statement.with_entity_field_name.data,
		statement.data.system_with_entity_statement.with_entity_field_name.length
	));

	EXPECT_EQ(read_amount, statement_str.size());
}
