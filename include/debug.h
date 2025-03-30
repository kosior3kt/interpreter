#ifndef __interpreter_debug__
#define __interpreter_debug__
//"../src/debug.c"

#include "common.h"
#include "chunk.h"

void disassemble_chunk(chunk_s*, const char*);
uint disassemble_instruction(chunk_s*, const uint);

#endif //__interpreter_debug__
