#include "gtest/gtest.h"

#include <ecsact/parse.h>
#include <string_view>
#include <string>

using namespace std::string_literals;

TEST(Parse, System) {
	auto statement_str = "system MySystem"s;
	ecsact_statement statement;
	ecsact_parse_status status;

	auto read_amount = ecsact_parse_statement(
		statement_str.data(),
		statement_str.size(),
		nullptr,
		&statement,
		&status
	);

	ASSERT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM);
	EXPECT_EQ(status.code, ECSACT_PARSE_STATUS_OK);

	EXPECT_EQ("MySystem", std::string_view(
		statement.data.system_statement.system_name.data,
		statement.data.system_statement.system_name.length
	));

	EXPECT_EQ(read_amount, statement_str.size());
}
