#ifndef __interpreter_chunk__
#define __interpreter_chunk__

#include "common.h"

typedef double value_t;
typedef uint8_t instruction_t;

////////////////////// helper types
typedef struct {
	uint size;
	uint capacity;
	value_t* data; //keep dynamic part at the end
}literals_array_s;

typedef enum: uint8_t {
	OP_UNDEFINED = 0,
	OP_RETURN,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NEGATION,
	OP_CONSTANT,
}opcode_e;

////////////////////// chunk

////////// variables
const static uint chunk_init_size = 128;	// cache line size on m1 macos

////////// types
typedef struct {
	uint size;
	uint capacity;
	literals_array_s literals;
	uint8_t* data;
	uint* lines;
}chunk_s;

///////// functions
void init_chunk(chunk_s*);
void free_chunk(chunk_s*);
void append_chunk(chunk_s*, const opcode_e, const uint);
int append_literal(chunk_s*, const value_t);


#endif //__interpreter_chunk__
