#include "gtest/gtest.h"

#include <string_view>
#include <string>
#include "magic_enum.hpp"
#include "ecsact/parse.h"

using namespace std::string_literals;

TEST(Parse, WithEntityStatement) {
	ecsact_statement context;
	context.type = ECSACT_STATEMENT_SYSTEM_COMPONENT;
	context.data.system_component_statement = {};

	auto                statement_str = "with test_entity;"s;
	ecsact_statement    statement;
	ecsact_parse_status status;

	auto read_amount = ecsact_parse_statement(
		statement_str.data(),
		statement_str.size(),
		&context,
		&statement,
		&status
	);

	ASSERT_EQ(status.code, ECSACT_PARSE_STATUS_OK)
		<< "status.code=" << magic_enum::enum_name(status.code) << "\n";
	ASSERT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM_WITH);

	ASSERT_EQ(statement.data.system_with_statement.with_field_name_list_count, 1);
	EXPECT_EQ(
		"test_entity",
		std::string_view(
			statement.data.system_with_statement.with_field_name_list[0].data,
			statement.data.system_with_statement.with_field_name_list[0].length
		)
	);

	EXPECT_EQ(read_amount, statement_str.size());
}
