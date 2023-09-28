#pragma once

#include "gtest/gtest.h"

#define STATEMENT_OK(OutStatement, Context, StatementString) \
	{                                                          \
		auto context = ecsact_statement{};                       \
		context.type = ECSACT_STATEMENT_##Context;               \
		context.data = {};                                       \
                                                             \
		auto statement_str = std::string{StatementString};       \
		auto status = ecsact_parse_status{};                     \
                                                             \
		auto read_amount = ecsact_parse_statement(               \
			statement_str.data(),                                  \
			statement_str.size(),                                  \
			&context,                                              \
			OutStatement,                                          \
			&status                                                \
		);                                                       \
                                                             \
		EXPECT_EQ(status.code, ECSACT_PARSE_STATUS_OK);          \
		EXPECT_EQ(read_amount, statement_str.size());            \
	}                                                          \
	static_assert(true, "Macro requires ;")

#define STATEMENT_BAD(Context, StatementString, BadStatus)   \
	{                                                          \
		auto context = ecsact_statement{};                       \
		auto statement = ecsact_statement{};                     \
		context.type = ECSACT_STATEMENT_##Context;               \
		context.data = {};                                       \
                                                             \
		auto statement_str = std::string{StatementString};       \
		auto status = ecsact_parse_status{};                     \
                                                             \
		auto read_amount = ecsact_parse_statement(               \
			statement_str.data(),                                  \
			statement_str.size(),                                  \
			&context,                                              \
			&statement,                                            \
			&status                                                \
		);                                                       \
                                                             \
		EXPECT_NE(status.code, ECSACT_PARSE_STATUS_OK);          \
		EXPECT_EQ(status.code, ECSACT_PARSE_STATUS_##BadStatus); \
		EXPECT_EQ(read_amount, 0);            \
	}                                                          \
	static_assert(true, "Macro requires ;")

#define EXPECT_STATEMENT_SV_EQ(Str, StatementSv) \
	EXPECT_EQ(Str, std::string_view(StatementSv.data, StatementSv.length))
