#include "../include/vm.h"

#define DEBUG_TRACE


//////////////////////// global vm state
vm_t vm;

//////////////////////// helper functions
static interpret_result_e run();

static value_t pop();
static void push(value_t);
static void reset_stack();

//////////////////////// implementations
void vm_init()
{
	vm.sp = vm.stack;
}

void vm_free()
{
	//nuthin to be done here (yet...)
}

interpret_result_e vm_interpret(const char* _code)
{

	compile();
	return INTERPRETER_OK;
}

//////////////////////// helper implementations
static interpret_result_e run()
{
	//it's this stupid ass syntax, that evaluates this shit and returns last thing in bracket
#define READ_BYTE()				(*vm.pc++)
#define READ_CONSTANT()			(vm.chunk->literals.data[READ_BYTE()])
#define BINARY_OPERATION(sign)	do {					\
									double b = pop();   \
									double a = pop();   \
									push(a sign b);	    \
								}while(0)

	interpret_result_e ret_val = INTERPRETER_UNDEFINED;
	instruction_t instruction;
	uint offset = 0;

	while((instruction = READ_BYTE()) != 0) {

#ifdef DEBUG_TRACE
		printf("	stack: [");
		for(value_t* value = vm.stack; value < vm.sp; ++value) {
			printf("%g, ", *value);
		}
		printf("]\n");
		offset = disassemble_instruction(vm.chunk, offset);
#endif
		switch(instruction)
		{
			case OP_RETURN: {
				printf("returning value: %g\n", pop());
				ret_val = INTERPRETER_OK;
				break;
			}
			case OP_CONSTANT: {
				value_t constant = READ_CONSTANT();
				push(constant);
				break;
			}
			case OP_ADD: {
				BINARY_OPERATION(+);
				break;
			}
			case OP_SUBTRACT: {
				BINARY_OPERATION(-);
				break;
			}
			case OP_MULTIPLY: {
				BINARY_OPERATION(*);
				break;
			}
			case OP_DIVIDE: {
				BINARY_OPERATION(/);
				break;
			}
			case OP_NEGATION: {
				push(-pop());
				break;
			}
		}
	}
	return ret_val;

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OPERATION
}

static value_t pop()
{
	if(vm.stack >= vm.sp) assert(0);
	--vm.sp;
	return *vm.sp;
}

static void push(value_t _value)
{
	if((vm.stack + STACK_MAX) <= vm.sp) assert(0);
	*vm.sp = _value;
	++vm.sp;
}

static void reset_stack()
{
	memset(vm.stack, '\0', STACK_MAX);
	vm.sp = vm.stack;
}

