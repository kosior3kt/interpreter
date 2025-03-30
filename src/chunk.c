#include "../include/chunk.h"

////////////////////////////////////////// static functions
static void realloc_chunk(chunk_s** _chunk);

///////////////// helpers
static void realloc_literals_array(literals_array_s**);
static void init_literals_array(literals_array_s*);
static void free_literals_array(literals_array_s*);
static int append_literals_array(literals_array_s*, const value_t);

////////////////////////////////////////// implementations
void init_chunk(chunk_s* _chunk)
{
	//TODO: check if aligned_alloc(); can be better here
	_chunk->data = (uint8_t*)malloc(sizeof(uint8_t) * chunk_init_size);
	memset(_chunk->data, '\0', chunk_init_size);
	_chunk->lines = (uint*)malloc(sizeof(uint) * chunk_init_size);
	memset(_chunk->lines, '\0', chunk_init_size);
	_chunk->capacity = chunk_init_size;
	_chunk->size = 0;
	init_literals_array(&_chunk->literals);
}

void free_chunk(chunk_s* _chunk)
{
	free_literals_array(&_chunk->literals);
	free(_chunk->data);
	free(_chunk->lines);
	_chunk->data = NULL; //is that needed?
	_chunk->lines = NULL; //is that needed?
}

void append_chunk(chunk_s* _chunk, const opcode_e _value_t, const uint _line_number)
{
	if(_chunk->capacity <= ++_chunk->size) {
		realloc_chunk(&_chunk);
	}
	_chunk->data[_chunk->size - 1] = _value_t;
	_chunk->lines[_chunk->size - 1] = _line_number;
}

int append_literal(chunk_s* _chunk, const value_t _value_t)
{
	return append_literals_array(&_chunk->literals, _value_t);
}

////////////////////////////////////////// static implementations
static void realloc_chunk(chunk_s** _chunk)
{
	(*_chunk)->data  = realloc((*_chunk)->data, (*_chunk)->capacity * 2);
	(*_chunk)->lines = realloc((*_chunk)->lines, (*_chunk)->capacity * 2);
	(*_chunk)->capacity *= 2;
	//TODO: find a way to init memory smartly here
}

/////////////////// helpers
static void realloc_literals_array(literals_array_s** _array)
{
	(*_array)->data = realloc((*_array)->data, (*_array)->capacity * 2);
	//no need to check. i mean what am i gonna to do if this fails anyways...
	(*_array)->capacity *= 2;
	//TODO: find a way to init memory smartly here
}

static void init_literals_array(literals_array_s* _array)
{
	//TODO: check if aligned_alloc(); can be better here
	_array->data = (value_t*)malloc(sizeof(value_t) * chunk_init_size);
	memset(_array->data, '\0', chunk_init_size);
	_array->capacity = chunk_init_size;
	_array->size = 0;
}

static void free_literals_array(literals_array_s* _array)
{
	free(_array->data);
}

static int append_literals_array(literals_array_s* _array, const value_t _value_t)
{
	if(_array->capacity <= ++_array->size) {
		realloc_literals_array(&_array);
	}
	_array->data[_array->size - 1] = _value_t;
	return _array->size - 1;
}
