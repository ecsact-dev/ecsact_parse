#include "gtest/gtest.h"

#include <string_view>
#include <string>
#include "ecsact/parse.h"

using namespace std::string_literals;

void TestValidSystemComponent(
	std::string_view         statement_str,
	ecsact_system_capability expected_capability,
	std::string_view         expected_component_name,
	std::string_view         expected_with_entity_field_name = ""
) {
	auto             system_name = "ExampleSystem"s;
	ecsact_statement component_statement{
		.type = ECSACT_STATEMENT_SYSTEM,
		.data{.system_statement{.system_name{
			.data = system_name.data(),
			.length = static_cast<int>(system_name.size()),
		}}},
	};

	ecsact_statement    statement;
	ecsact_parse_status status;

	auto read_amount = ecsact_parse_statement(
		statement_str.data(),
		statement_str.size(),
		&component_statement,
		&statement,
		&status
	);

	ASSERT_EQ(statement.type, ECSACT_STATEMENT_SYSTEM_COMPONENT);
	EXPECT_EQ(status.code, ECSACT_PARSE_STATUS_OK);

	EXPECT_EQ(
		expected_capability,
		statement.data.system_component_statement.capability
	);
	EXPECT_EQ(
		expected_component_name,
		std::string_view(
			statement.data.system_component_statement.component_name.data,
			statement.data.system_component_statement.component_name.length
		)
	);

	EXPECT_EQ(
		expected_with_entity_field_name,
		std::string_view(
			statement.data.system_component_statement.with_entity_field_name.data,
			statement.data.system_component_statement.with_entity_field_name.length
		)
	);

	EXPECT_EQ(read_amount, statement_str.size());
}

TEST(Parse, ReadonlySystemComponent) {
	TestValidSystemComponent(
		"readonly ExampleComponent;"s,
		ECSACT_SYS_CAP_READONLY,
		"ExampleComponent"
	);
}

TEST(Parse, WriteonlySystemComponent) {
	TestValidSystemComponent(
		"writeonly ExampleComponent;"s,
		ECSACT_SYS_CAP_WRITEONLY,
		"ExampleComponent"
	);
}

TEST(Parse, ReadwriteSystemComponent) {
	TestValidSystemComponent(
		"readwrite ExampleComponent;"s,
		ECSACT_SYS_CAP_READWRITE,
		"ExampleComponent"
	);
}

TEST(Parse, OptionalReadonlySystemComponent) {
	TestValidSystemComponent(
		"optional readonly ExampleComponent;"s,
		ECSACT_SYS_CAP_OPTIONAL_READONLY,
		"ExampleComponent"
	);
}

TEST(Parse, OptionalWriteonlySystemComponent) {
	TestValidSystemComponent(
		"optional writeonly ExampleComponent;"s,
		ECSACT_SYS_CAP_OPTIONAL_WRITEONLY,
		"ExampleComponent"
	);
}

TEST(Parse, OptionalReadwriteSystemComponent) {
	TestValidSystemComponent(
		"optional readwrite ExampleComponent;"s,
		ECSACT_SYS_CAP_OPTIONAL_READWRITE,
		"ExampleComponent"
	);
}

TEST(Parse, IncludeSystemComponent) {
	TestValidSystemComponent(
		"include ExampleComponent;"s,
		ECSACT_SYS_CAP_INCLUDE,
		"ExampleComponent"
	);
}

TEST(Parse, ExcludeSystemComponent) {
	TestValidSystemComponent(
		"exclude ExampleComponent;"s,
		ECSACT_SYS_CAP_EXCLUDE,
		"ExampleComponent"
	);
}

TEST(Parse, AddsSystemComponent) {
	TestValidSystemComponent(
		"adds ExampleComponent;"s,
		ECSACT_SYS_CAP_ADDS,
		"ExampleComponent"
	);
}

TEST(Parse, RemovesSystemComponent) {
	TestValidSystemComponent(
		"removes ExampleComponent;"s,
		ECSACT_SYS_CAP_REMOVES,
		"ExampleComponent"
	);
}

///////////////////////////////////
// 'with' keyword                //
///////////////////////////////////

TEST(Parse, ReadonlySystemComponentWithEntity) {
	TestValidSystemComponent(
		"readonly ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_READONLY,
		"ExampleComponent",
		"example_entity"
	);
}

TEST(Parse, WriteonlySystemComponentWithEntity) {
	TestValidSystemComponent(
		"writeonly ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_WRITEONLY,
		"ExampleComponent",
		"example_entity"
	);
}

TEST(Parse, ReadwriteSystemComponentWithEntity) {
	TestValidSystemComponent(
		"readwrite ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_READWRITE,
		"ExampleComponent",
		"example_entity"
	);
}

TEST(Parse, OptionalReadonlySystemComponentWithEntity) {
	TestValidSystemComponent(
		"optional readonly ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_OPTIONAL_READONLY,
		"ExampleComponent",
		"example_entity"
	);
}

TEST(Parse, OptionalWriteonlySystemComponentWithEntity) {
	TestValidSystemComponent(
		"optional writeonly ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_OPTIONAL_WRITEONLY,
		"ExampleComponent",
		"example_entity"
	);
}

TEST(Parse, OptionalReadwriteSystemComponentWithEntity) {
	TestValidSystemComponent(
		"optional readwrite ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_OPTIONAL_READWRITE,
		"ExampleComponent",
		"example_entity"
	);
}

TEST(Parse, IncludeSystemComponentWithEntity) {
	TestValidSystemComponent(
		"include ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_INCLUDE,
		"ExampleComponent",
		"example_entity"
	);
}

////////////////////////////////////
// components from other packages //
////////////////////////////////////

TEST(Parse, OtherPackageFullyQualified) {
	TestValidSystemComponent(
		"readwrite other.pkg.ExampleComponent;"s,
		ECSACT_SYS_CAP_READWRITE,
		"other.pkg.ExampleComponent"
	);
}

TEST(Parse, IncludeSystemComponentWithEntityFullQualified) {
	TestValidSystemComponent(
		"include other.pkg.ExampleComponent with example_entity;"s,
		ECSACT_SYS_CAP_INCLUDE,
		"other.pkg.ExampleComponent",
		"example_entity"
	);
}
