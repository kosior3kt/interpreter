#include "../include/scanner.h"

typedef struct {
	const char* start;
	const char* current;
	uint line;
}scanner_s;

static scanner_s scanner;

static bool reached_end();
static token_s make_token(const token_type_e);
static token_s make_error_token(const char*);
static token_s make_string();
static token_s make_number();
static token_s make_identifier();
static char advance();
static bool match(const char);
static void skip_withespace();
static char peek();
static char peek_next();
static bool is_digit(const char);
static bool is_alpha(const char);
static token_type_e identifier_type();
static token_type_e check_keyword(int, int, const char*, const token_type_e);

void init_scanner(const char* _code)
{
	scanner.start	= _code;
	scanner.current = _code;
	scanner.line = 1;
}

token_s scan_token()
{
	skip_withespace();
	scanner.start = scanner.current;

	if(reached_end()) return make_token(TOKEN_EOF);

	char c = advance();
	if(is_alpha(c)) return make_identifier();
	if(is_digit(c)) return make_number();

	switch(c) {
		case '(': return make_token(TOKEN_LEFT_PAREN);
		case ')': return make_token(TOKEN_RIGHT_PAREN);
		case '{': return make_token(TOKEN_LEFT_BRACE);
		case '}': return make_token(TOKEN_RIGHT_BRACE);
		case ';': return make_token(TOKEN_SEMICOLON);
		case ',': return make_token(TOKEN_COMMA);
		case '.': return make_token(TOKEN_DOT);
		case '-': return make_token(TOKEN_MINUS);
		case '+': return make_token(TOKEN_PLUS);
		case '/': return make_token(TOKEN_SLASH);
		case '*': return make_token(TOKEN_STAR);
		case '!':
			return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '"':
			return make_string();
	}

	return make_error_token("unexpected character");
}

static bool reached_end()
{
	return *scanner.current == '\0';
}

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
	ret_val.length = (uint)(strlen(_error_message));
	ret_val.line   = scanner.line;

	return ret_val;
}

static inline char advance()
{
	return (-1)[++scanner.current];
}

static bool match(const char _char)
{
	if(reached_end) return false;
	if(scanner.current[1] != _char) return false;

	++scanner.current;
	return true;
}

static void skip_withespace()
{
	while(1) {
		switch(peek()) {
			case ' ':
			case '\t':
			case '\r': {
				advance();
				break;
			}
			case '\n': {
				++scanner.line;
				advance();
				break;
			}
			case '/': {
				if(peek_next() == '/') {
					while(peek() != '\n' && !reached_end()) advance();
				} else {
					return;
				}
				break;
			}
			default: return;
		}
	}
}

static char peek()
{
	return *scanner.current;
}

static char peek_next()
{
	if(reached_end()) return '\0';
	return *(scanner.current + 1);
}

static token_s make_string()
{
	while(peek() != '"' && !reached_end()) {
		if(peek() == '\n') ++scanner.line;
		advance();
	}

	if(reached_end()) return make_error_token("Undetermined string - reached end while parsing string\n");

	advance();
	return make_token(TOKEN_STRING);
}

static token_s make_identifier()
{
	while(is_alpha(peek()) || is_digit(peek())) advance();
	return make_token(identifier_type());
}

static token_s make_number()
{
	while(is_digit(peek())) advance();

	if(peek() == '.' && is_digit(peek_next())) {
		advance();
		while(is_digit(peek())) advance();
	}

	return make_token(TOKEN_NUMBER);
}

static bool is_digit(const char _char)
{
	return (_char >= '0' && _char <= '9');
}

static bool is_alpha(const char _char)
{
	return ((_char >= 'a' && _char <= 'z') || (_char >= 'A' && _char <= 'Z') || _char == '_');
}

static token_type_e identifier_type()
{

	switch (scanner.start[0]) {
		case 'a': return check_keyword(1, 2, "nd", TOKEN_AND);
		case 'c': return check_keyword(1, 4, "lass", TOKEN_CLASS);
		case 'e': return check_keyword(1, 3, "lse", TOKEN_ELSE);
		case 'i': return check_keyword(1, 1, "f", TOKEN_IF);
		case 'n': return check_keyword(1, 2, "il", TOKEN_NIL);
		case 'o': return check_keyword(1, 1, "r", TOKEN_OR);
		case 'p': return check_keyword(1, 4, "rint", TOKEN_PRINT);
		case 'r': return check_keyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': return check_keyword(1, 4, "uper", TOKEN_SUPER);
		case 'v': return check_keyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return check_keyword(1, 4, "hile", TOKEN_WHILE);
		case 'f': {
			if(scanner.start - scanner.current > 0) {
				switch(scanner.start[1]) {
					case 'a': return check_keyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return check_keyword(2, 1, "r", TOKEN_FOR);
					case 'u': return check_keyword(2, 1, "n", TOKEN_FUN);
				}
			}
		}
		case 't': {
			if(scanner.start - scanner.current > 0) {
				switch(scanner.start[1]) {
					case 'h': return check_keyword(2, 2, "is", TOKEN_THIS);
					case 'r': return check_keyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
		}
	}

	return TOKEN_IDENTIFIER;
}


static token_type_e check_keyword(int _start, int _len, const char* _rest, const token_type_e _potential_type)
{
	if(scanner.current - scanner.start == _start + _len && memcmp(scanner.start + _start, _rest, _len) == 0) {
		return _potential_type;
	} else {
		return TOKEN_IDENTIFIER;
	}
}

