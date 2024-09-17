#pragma once
#include <string>

//	Command-Line Arguments
const std::string UNSET_FILE_NAME = "UNSET";
const std::string ASSEMBLY_FILE_ENDING = "asm";

#define	BINARY_FLAG_NAME		"b"
#define OUTPUT_FILE_FLAG_NAME	"o"

//	Parsing
#define	UNDERSCORE				'_'
#define	SPECIAL_REGISTER_PREFIX	'%'
#define	IMMEDIATE_PREFIX		'#'
#define	DIRECTIVE_PREFIX		'.'
#define	LABEL_PREFIX			':'
const std::string COMMENT_START = "//";