#ifndef __interpreter_vm__
#define __interpreter_vm__
// "../src/vm.c"

#include "chunk.h"
#include "common.h"

#define STACK_MAX 1024 //hell yeah - 1kb!

typedef enum {
	INTERPRETER_UNDEFINED = 0,
	INTERPRETER_OK,
	INTERPRETER_COMPILER_ERROR,
	INTERPRETER_RUNTIME_ERROR,
}interpret_result_e;

typedef struct {
	chunk_s* chunk;
	uint8_t* pc;
	value_t  stack[STACK_MAX];
	value_t* sp;
}vm_t;

void vm_init();
void vm_free();
interpret_result_e vm_interpret(const char*);

#endif //__interpreter_vm__
