#ifndef ECSACT_PARSE_ERROR_H
#define ECSACT_PARSE_ERROR_H

#include "./string.h"

typedef enum {
	ECSACT_PARSE_ERR_STATUS,
	ECSACT_PARSE_ERR_EXPECTED_PACKAGE_STATEMENT,
} ecsact_parse_error_code;

typedef struct {
	ecsact_parse_error_code code;
	ecsact_statement_sv error_location;
} ecsact_parse_error;

#endif// ECSACT_PARSE_ERROR_H
