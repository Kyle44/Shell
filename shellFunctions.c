// Name: Kyle Fritz
// File: main.c
// Date Created: 2/20/16
// Description: main file for my own shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIM " /t/r/n/a"

int startProgram(char** args){
	pid_t pid, wpid;
	int status;
	
	pid = fork();
	if(pid == 0){
//		if(execl





}

char* readLine(){
	int currentBufferSize = BUFFER_SIZE;
	int position = 0;
	char* buffer = malloc(sizeof(char) * currentBufferSize);
	int c;

	if(!buffer){
		fprintf(stderr, "Allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1){
		c = getchar(); // Read a character

		if(c == EOF || c == '\n'){
			buffer[position] = '\0';
			return buffer;
		}
 
		else{
			buffer[position] = c;
		}
		position++;

		if(position >= currentBufferSize){
			currentBufferSize += BUFFER_SIZE;
			buffer = realloc(buffer, currentBufferSize);
			if(!buffer){
				fprintf(stderr, "Reallocation Error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char** splitLine(char* line){
	int currentTokenBufferSize = TOKEN_BUFFER_SIZE;
	int position = 0;
	char** tokens = malloc(currentTokenBufferSize * sizeof(char*));
	char* token;

	if(!tokens){
		fprintf(stderr, "Token Allocation Error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOKEN_DELIM);
	while (token != NULL){
		tokens[position] = token;
		position++;
	
		if(position >= currentTokenBufferSize){

			currentTokenBufferSize += TOKEN_BUFFER_SIZE;
			tokens = realloc(tokens, currentTokenBufferSize * sizeof(char*));
			if(!tokens){
				fprintf(stderr, "Token Reallocation Error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, TOKEN_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}


