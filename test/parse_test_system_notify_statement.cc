#include "gtest/gtest.h"

#include <string_view>
#include <string>
#include "ecsact/parse.h"

#include "parse_test_util.hh"

using namespace std::string_literals;

TEST(Parse, SystemNotifyShorthand) {
	auto statement = ecsact_statement{};
	STATEMENT_OK(&statement, SYSTEM, "notify always;");

	EXPECT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM_NOTIFY);

	EXPECT_STATEMENT_SV_EQ(
		"always",
		statement.data.system_notify_statement.setting_name
	);
}

TEST(Parse, SystemNotifyLongForm) {
	auto statement = ecsact_statement{};
	STATEMENT_OK(&statement, SYSTEM_NOTIFY, "always ExampleComponent;");

	EXPECT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM_NOTIFY_COMPONENT);

	EXPECT_STATEMENT_SV_EQ(
		"always",
		statement.data.system_notify_component_statement.setting_name
	);
	EXPECT_STATEMENT_SV_EQ(
		"ExampleComponent",
		statement.data.system_notify_component_statement.component_name
	);


	STATEMENT_OK(&statement, SYSTEM_NOTIFY, "onchange AnotherComponent;");

	EXPECT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM_NOTIFY_COMPONENT);

	EXPECT_STATEMENT_SV_EQ(
		"onchange",
		statement.data.system_notify_component_statement.setting_name
	);
	EXPECT_STATEMENT_SV_EQ(
		"AnotherComponent",
		statement.data.system_notify_component_statement.component_name
	);
}
