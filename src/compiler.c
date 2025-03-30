#include "../include/compiler.h"

void compile(const char* _code)
{
	init_scanner(_code);
	int line = -1;
	while(1)
	{
		token_s token = scan_token();
		if(token.line != line) {
			printf("%4 ", token.line);
			line = token.line;
		} else {
			printf("  |  ");
		}
		printf("%2d '%10s'\n", token.type, token.start);

		if(token.type == TOKEN_EOF) break;
	}
}

