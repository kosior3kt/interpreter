#include "../include/common.h"
#include "../include/chunk.h"
#include "../include/debug.h"
#include "../include/vm.h"

static char* read_file(const char*);

static void repl();
static void run_file(const char*);

int main(int argc, char** argv)
{
	vm_init();

	if(argc == 1) {
		repl();
	} else if(argc == 2) {
		const char* file_name = *(argv + 1);
		run_file(file_name);
	} else {
		printf("usage: prog [file_name]\n");
		exit(-1);
	}

	vm_free();
	return 0;
}

static void repl()
{
	char line[1024];
	while(true)
	{
		printf("> ");
		if(!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}

		vm_interpret(line);
	}
}

static void run_file(const char* _file_name)
{
	char* source_code = read_file(_file_name);
	interpret_result_e result = vm_interpret(source_code);
	free(file_contents);

	if(result == INTERPRETER_COMPILER_ERROR) exit(65);
	if(result == INTERPRETER_RUNTIME_ERROR) exit(70);
}

static char* read_file(const char* _file_name)
{
	FILE* file = fopen(_file_name, "r");
	if(!file) {
		perror("error reading file:");
		exit(74);
	}
	(void)fseek(file, 0L, SEEK_END);
	int file_size = ftell(file);
	rewind(file);

	char* file_contents = (char*)malloc(sizeof(char) * file_size + 1);
	int bytes_read = fread(file_contents, sizeof(char), file_size, file);
	file_contents[bytes_read] = '\0';

	close(file);
	return file_contents;
}

