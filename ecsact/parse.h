#ifndef ECSACT_PARSER_H
#define ECSACT_PARSER_H

#include "ecsact/parse/statements.h"
#include "ecsact/parse/status.h"
#include "ecsact/parse/error.h"

typedef enum {
	ECSACT_PARSE_CALLBACK_STOP,
	ECSACT_PARSE_CALLBACK_CONTINUE,
} ecsact_parse_callback_result;

typedef struct {
	const ecsact_parse_error* error;
	const ecsact_package_statement* package;
	const ecsact_statement* context_statement;
	const ecsact_statement* statement;
	// Index of source file in the list of source files given initially. If no
	// source files were given then the value will be `-1`
	int source_file_index;
} ecsact_parse_callback_params;

typedef ecsact_parse_callback_result(*ecsact_parse_callback)
	( ecsact_parse_callback_params  params
	, void*                         callback_user_data
	);

/**
 * Reads a single statement outputting the parse status and the resulting 
 * statement.
 * @param statement_string the string that is parsed
 * @param max_read_length the length of `statement_string`
 * @param context_statement contextual statement that should be considered while
 *        parsing `statement_string`. May be `NULL`.
 * @returns read length
 */
int ecsact_parse_statement
	( const char*              statement_string
	, int                      max_read_length
	, const ecsact_statement*  context_statement
	, ecsact_statement*        out_statement
	, ecsact_parse_status*     out_status
	);

void ecsact_parse
	( const char**           file_paths
	, int                    file_paths_count
	, ecsact_parse_callback  callback
	, void*                  callback_user_data
	);

#ifdef __cplusplus
template<typename CallbackPtr>
ecsact_parse_callback_result ecsact_parse_cpp_callback_invoker
	( ecsact_parse_callback_params  params
	, void*                         callback_user_data
	)
{
	return (*reinterpret_cast<CallbackPtr>(callback_user_data))(params);
}

template<typename Callback>
void ecsact_parse_with_cpp_callback
	( const char**   file_paths
	, int            file_paths_count
	, Callback&&     callback
	)
{
	using callback_ptr_t = Callback*;
	callback_ptr_t callback_ptr = &callback;

	ecsact_parse(
		file_paths,
		file_paths_count,
		&ecsact_parse_cpp_callback_invoker<callback_ptr_t>,
		callback_ptr
	);
}

template<typename Callback>
void ecsact_parse_statement_cpp_visit
	( const ecsact_statement&  statement
	, Callback&&               callback
	)
{
	switch(statement.type) {
		case ECSACT_STATEMENT_NONE:
			callback(statement.type, ecsact_none_statement{});
			break;
		case ECSACT_STATEMENT_UNKNOWN:
			callback(statement.type, ecsact_unknown_statement{});
			break;
		case ECSACT_STATEMENT_PACKAGE:
			callback(statement.type, statement.data.package_statement);
			break;
		case ECSACT_STATEMENT_IMPORT:
			callback(statement.type, statement.data.import_statement);
			break;
		case ECSACT_STATEMENT_COMPONENT:
			callback(statement.type, statement.data.component_statement);
			break;
		case ECSACT_STATEMENT_SYSTEM:
			callback(statement.type, statement.data.system_statement);
			break;
		case ECSACT_STATEMENT_ACTION:
			callback(statement.type, statement.data.action_statement);
			break;
		case ECSACT_STATEMENT_ENUM:
			callback(statement.type, statement.data.enum_statement);
			break;
		case ECSACT_STATEMENT_I8_FIELD:
		case ECSACT_STATEMENT_U8_FIELD:
		case ECSACT_STATEMENT_I16_FIELD:
		case ECSACT_STATEMENT_U16_FIELD:
		case ECSACT_STATEMENT_I32_FIELD:
		case ECSACT_STATEMENT_U32_FIELD:
		case ECSACT_STATEMENT_F32_FIELD:
		case ECSACT_STATEMENT_ENTITY_FIELD:
			callback(statement.type, statement.data.field_statement);
			break;
		case ECSACT_STATEMENT_USER_TYPE_FIELD:
			callback(statement.type, statement.data.user_type_field_statement);
			break;
		case ECSACT_STATEMENT_SYSTEM_COMPONENT:
			callback(statement.type, statement.data.system_component_statement);
			break;
		// case ECSACT_STATEMENT_SYSTEM_GENERATES:
		// 	callback(statement.type, statement.data.system_generates_statement);
		// 	break;
		case ECSACT_STATEMENT_SYSTEM_WITH_ENTITY:
			callback(statement.type, statement.data.system_with_entity_statement);
			break;
		case ECSACT_STATEMENT_ENTITY_CONSTRAINT:
			callback(statement.type, statement.data.entity_constraint_statement);
			break;
	}
}
#endif

#endif // ECSACT_PARSER_H
