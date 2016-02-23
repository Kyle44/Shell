// Name: Kyle Fritz
// File: main.c
// Date Created: 2/20/16
// Description: main file for my own shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shell(){
	char prompt = '$';
	char* line;
	char** args;
	int status;

	do{
		printf("%c", prompt);
		line = readLine();
		args = splitLine(line);
		status = execute(args);

		free(line);
		free(args);
	} while(status);	
}

int main(int argc, char *argv[])
{
	shell();
}
