#!/bin/bash

gcc -o build/prog -g src/main.c src/chunk.c src/debug.c src/vm.c src/compiler.c src/scanner.c

if [[ "$1" == "run" && "$?" == 0 ]]; then
	clear
	./build/prog
fi
