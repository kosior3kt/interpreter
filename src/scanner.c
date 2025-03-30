#include "../include/scanner.h"

typedef struct {
	const char* start;
	const char* current;
	uint line;
}scanner_s;

static scanner_s scanner;

static int reached_end();
static token_s make_token(const token_type_e);
static token_s make_error_token(const char*);
static char advance();
static char match(const char);


void init_scanner(const char* _code)
{
	scanner.start	= _code;
	scanner.current = _code;
	line = 1;
}

token_s scan_token()
{
	scanner.start = scanner.current;

	if(reached_end()) return make_token(TOKEN_EOF);

	char c = advance();

	switch(c)
	{
		case '(': return make_token(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);
		case '!':
			return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
	}

	return make_error_token("unexpected character");
}



static int reached_end()
{
	return *scanner.current == '\0';
}

typedef struct {
	token_type_e type;
	const char* start;
	uint length;
	uint line;
}token_s;

static token_s make_token(const token_type_e _token_type)
{
	token_s ret_val;

	ret_val.type   = _token_type;
	ret_val.start  = scanner.start;
	ret_val.length = (uint)(scanner.current - scanner.start);
	ret_val.line   = scanner.line;

	return ret_val;
}

static token_s make_error_token(const char* _error_message)
{
	token_s ret_val;

	ret_val.type   = TOKEN_ERROR;
	ret_val.start  = _error_message;
	ret_val.length = (uint)(strlen _error_message);
	ret_val.line   = scanner.line;

	return ret_val;
}

static char advance()
{
	return (-1)[++scanner.current];
}

static char match(const char* _char)
{
	if(reached_end) return false;
	if(scanner.current[1] != _char) return false;

	++scanner.current;
	return true;
}

