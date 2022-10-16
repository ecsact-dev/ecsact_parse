#ifndef ECSACT_PARSE_STATEMENTS_H
#define ECSACT_PARSE_STATEMENTS_H

#include <stdint.h>
#include "ecsact/parse/string.h"
#include "ecsact/runtime/common.h"
#include "ecsact/runtime/definitions.h"

typedef enum {
	ECSACT_STATEMENT_NONE,
	ECSACT_STATEMENT_UNKNOWN,
	ECSACT_STATEMENT_PACKAGE,
	ECSACT_STATEMENT_IMPORT,
	ECSACT_STATEMENT_COMPONENT,
	ECSACT_STATEMENT_TRANSIENT,
	ECSACT_STATEMENT_SYSTEM,
	ECSACT_STATEMENT_ACTION,
	ECSACT_STATEMENT_ENUM,
	ECSACT_STATEMENT_ENUM_VALUE,
	ECSACT_STATEMENT_BUILTIN_TYPE_FIELD,
	ECSACT_STATEMENT_USER_TYPE_FIELD,
	ECSACT_STATEMENT_ENTITY_FIELD,
	ECSACT_STATEMENT_SYSTEM_COMPONENT,
	ECSACT_STATEMENT_SYSTEM_GENERATES,
	ECSACT_STATEMENT_SYSTEM_WITH_ENTITY,
	ECSACT_STATEMENT_ENTITY_CONSTRAINT,
} ecsact_statement_type;

typedef struct {
} ecsact_none_statement;

typedef struct {
} ecsact_unknown_statement;

typedef struct {
	bool main;
	ecsact_statement_sv package_name;
} ecsact_package_statement;

typedef struct {
	ecsact_statement_sv import_package_name;
} ecsact_import_statement;

typedef struct {
	ecsact_statement_sv component_name;
} ecsact_component_statement;

typedef struct {
	ecsact_statement_sv transient_name;
} ecsact_transient_statement;

typedef struct {
	ecsact_statement_sv system_name;
} ecsact_system_statement;

typedef struct {
	ecsact_statement_sv action_name;
} ecsact_action_statement;

typedef struct {
	ecsact_statement_sv enum_name;
} ecsact_enum_statement;

typedef struct {
	ecsact_statement_sv name;
	int32_t value;
} ecsact_enum_value_statement;

typedef struct {
	ecsact_builtin_type field_type;
	ecsact_statement_sv field_name;
	int length;
} ecsact_field_statement;

typedef struct {
	ecsact_statement_sv user_type_name;
	ecsact_statement_sv field_name;
	int length;
} ecsact_user_type_field_statement;

typedef struct {
	ecsact_system_capability capability;
	ecsact_statement_sv component_name;
	ecsact_statement_sv with_entity_field_name;
} ecsact_system_component_statement;

typedef struct {
	ecsact_statement_sv with_entity_field_name;
} ecsact_system_with_entity_statement;

typedef struct {
	bool optional;
	ecsact_statement_sv constraint_component_name;
} ecsact_entity_constraint_statement;

typedef union {
	ecsact_package_statement package_statement;
	ecsact_import_statement import_statement;
	ecsact_component_statement component_statement;
	ecsact_transient_statement transient_statement;
	ecsact_system_statement system_statement;
	ecsact_action_statement action_statement;
	ecsact_enum_statement enum_statement;
	ecsact_enum_value_statement enum_value_statement;
	ecsact_field_statement field_statement;
	ecsact_user_type_field_statement user_type_field_statement;
	ecsact_system_component_statement system_component_statement;
	ecsact_system_with_entity_statement system_with_entity_statement;
	ecsact_entity_constraint_statement entity_constraint_statement;
} ecsact_statement_data;

typedef struct ecsact_statement {
	/**
	 * Unique identifier for this statement.
	 */
	int32_t id;

	/**
	 * Statement type enum used to match correct `data` union member
	 */
	ecsact_statement_type type;

	/**
	 * Valid union member determined by `type`
	 */
	ecsact_statement_data data;
} ecsact_statement;

#endif // ECSACT_PARSE_STATEMENTS_H
