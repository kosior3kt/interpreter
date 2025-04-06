#include "../include/compiler.h"
#include "../include/scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "../include/debug.h"
#endif

typedef struct {
	token_s current;
	token_s previous;
	bool had_error;
	bool panic_mode;
}parser_s;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
}precedence_e;

typedef void(*parse_fn_t)();

typedef struct {
	parse_fn_t prefix;
	parse_fn_t infix;
	precedence_e precedence;
}parse_rule_s;


static void expression();
static void number();
static void grouping();
static void unary();
static void binary();

static const parse_rule_s rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};



static parser_s parser; //TODO: can this be static?
static chunk_s* compiling_chunk;

static void init_module(chunk_s*);
static void advance();
static void error_at_current(const char*);
static void error(const char*);
static void error_at(token_s*, const char*);
static void consume(const token_type_e, const char*);
static void emit_byte(const uint8_t);
static void emit_bytes(const uint8_t, const uint8_t);
static chunk_s* current_chunk();
static void end_compiler();
static void emit_return();
static void emit_constant(const double);
static void parse_precedence(const precedence_e);

static uint8_t make_constant(const double);

static const parse_rule_s* get_rule(const token_type_e);

bool compile(const char* _code, chunk_s* _chunk)
{
	init_module(_chunk);
	init_scanner(_code);
	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression\n");
	end_compiler();
	// return false on error.
	return !parser.had_error;
}


static void advance()
{
	parser.previous = parser.current;
	while(1) {
		parser.current = scan_token();
		if(parser.current.type != TOKEN_ERROR) break;
		error_at_current(parser.current.start);
	}
}

static void init_module(chunk_s* _chunk)
{
	parser.had_error  = false;
	parser.panic_mode = false;
	compiling_chunk	  = _chunk;
}

static void error_at_current(const char* _message)
{
	error_at(&parser.current, _message);
}

static void error(const char* _message)
{
	error_at(&parser.previous, _message);
}

static void error_at(token_s* _token, const char* _message)
{
	if(parser.panic_mode) return;
	parser.panic_mode = true;

	fprintf(stderr, "[line %d] Error", _token->line);
	if(_token->type == TOKEN_EOF) {
		fprintf(stderr, "at the end");
	} else if(_token->type == TOKEN_ERROR) {
		//nuthin...
	} else {
		fprintf(stderr, "at %.*s", _token->length, _token->start);
	}

	fprintf(stderr, ": %s\n", _message);
	parser.had_error = true;
}

static void consume(const token_type_e _token_type, const char* _message)
{
	if(parser.current.type == _token_type) {
		advance();
		return;
	}
	error_at_current(_message);
}

static void emit_byte(const uint8_t _byte)
{
	append_chunk(current_chunk(), _byte, parser.previous.line);
}

static void emit_bytes(const uint8_t _byte, const uint8_t _byte2)
{
	emit_byte(_byte);
	emit_byte(_byte2);
}

static chunk_s* current_chunk()
{
	return compiling_chunk;
}

static void end_compiler()
{
#ifdef DEBUG_PRINT_CODE
	if(!parser.had_error) {
		disassemble_chunk(current_chunk, "code");
	}
#endif
	emit_return();
}

static void emit_return()
{
	emit_byte(OP_RETURN);
}

static void emit_constant(const double _val)
{
	emit_bytes(OP_CONSTANT, make_constant(_val));
}

static uint8_t make_constant(const double _val)
{
	uint8_t index = append_literal(current_chunk(), _val);

	if(index > UINT8_MAX) {
		perror("too many constants\n");
		return 0;
	}
	return index;
}

static void expression()
{
	parse_precedence(PREC_ASSIGNMENT);
}

static void number()
{
	double value = strtod(parser.previous.start, NULL);
	emit_constant(value);
}

static void grouping()
{
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

static void unary()
{
	token_type_e operator_type = parser.previous.type;

	parse_precedence(PREC_UNARY);

	switch(operator_type) {
		case TOKEN_MINUS: emit_byte(OP_NEGATION); break;
		default: return;
	}
}

static void binary()
{
	token_type_e operator_type = parser.previous.type;
	const parse_rule_s* rule = get_rule(operator_type);
	parse_precedence((precedence_e)(rule->precedence + 1));

	switch(operator_type) {
		case TOKEN_PLUS:  emit_byte(OP_ADD);	  break;
		case TOKEN_MINUS: emit_byte(OP_SUBTRACT); break;
		case TOKEN_STAR:  emit_byte(OP_MULTIPLY); break;
		case TOKEN_SLASH: emit_byte(OP_DIVIDE);   break;
		default: return;
	}
	return;
}

static void parse_precedence(const precedence_e _precedence)
{
	advance();
	parse_fn_t prefix_rule = get_rule(parser.previous.type)->prefix;
	if(prefix_rule == NULL) {
		error("expexted expression");
		return;
	}

	prefix_rule();

	while(_precedence <= get_rule(parser.current.type)->precedence) {
		advance();
		parse_fn_t infix_rule = get_rule(parser.previous.type)->infix;
		infix_rule();
	}
}


static const parse_rule_s* get_rule(const token_type_e _type)
{
	return &rules[_type];
}
