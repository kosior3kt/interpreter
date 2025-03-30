#include "../include/debug.h"


//////////// static variables
static const char* padding = "                             ";

//////////// static functions
static void print_zero_operands(const char* _name)
{
	printf("%-10s\n", _name);
}

static void print_one_operand(const char* _name,
							   const value_t _operand)
{
	printf("%-10s", _name);
	printf(" oper: %g\n", _operand);
}

static void print_two_operands(const char* _name,
							   const value_t _operand_1,
							   const value_t _operand_2)
{
	printf("%-10s", _name);
	printf(" opers: %g, %g\n", _operand_1, _operand_2);
}

uint disassemble_instruction(chunk_s* _chunk, const uint _offset)
{
	//redue this later maybe?
	uint instruction_size = 0;

	printf("%04d: ", _offset);

	if(_offset > 0 && _chunk->lines[_offset] == _chunk->lines[_offset - 1])
		printf(" |  ");
	else
		printf("%03d ", _chunk->lines[_offset]);

	switch(_chunk->data[_offset])
	{
		case OP_UNDEFINED: {
			instruction_size = 1;
			print_zero_operands("undefined operation");
			break;
		}
		case OP_RETURN: {
			instruction_size = 1;
			print_zero_operands("return");
			break;
		}
		case OP_CONSTANT: {
			instruction_size = 2;
			print_one_operand("constant", _chunk->literals.data[_chunk->data[_offset + 1]]);
			break;
		}
		case OP_ADD: {
			instruction_size = 1;
			print_zero_operands("add");
			break;
		}
		case OP_SUBTRACT: {
			instruction_size = 1;
			print_zero_operands("subtract");
			break;
		}
		case OP_MULTIPLY: {
			instruction_size = 1;
			print_zero_operands("multiply");
			break;
		}
		case OP_DIVIDE: {
			instruction_size = 1;
			print_zero_operands("divide");
			break;
		}
		case OP_NEGATION: {
			instruction_size = 1;
			print_zero_operands("negation");
			break;
		}
	}

	// this will vary when we introduce operands
	return (_offset + instruction_size);
}

//////////// implementations
void disassemble_chunk(chunk_s* _chunk, const char* _title)
{
	printf("====== %s [start] ======\n", _title);
	uint offset = 0;
	while(offset < _chunk->size)
		offset = disassemble_instruction(_chunk, offset);
	printf("====== %s [end] ========\n", _title);
}

