#!/bin/bash

clear
echo "Make sure to use bash"

set -e #if error occures => exiting

gcc test.c -o bin/test.exe -lm -Wall -Wextra -g
time ./bin/test.exe

#for file in programs/*.c
#do
#	filename=${file#*/}		#cut of 'programs/'
#	echo -e "[\e[96mCompiling\e[0m] $filename ..."
#	gcc $file -o "bin/"${filename/".c"/".exe"} -lm
#	#gcc -shared $file -o "lib/"${filename/".c"/".so"} -lm -fPIC
#done