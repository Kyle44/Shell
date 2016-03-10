// Name: Kyle Fritz
// File: shellFunctions.c
// Date Created: 2/20/16
// Description: Functionality for main shell

#include "builtinFuncs.c"


int execute(char** args){
        if(args[0] == NULL){
                return 1;  // Nothing entered, so return to command prompt
        }

        char* builtinCommands[] = {"exit", "chdir", "cd", "getenv", "setebv", "echo"};
        int (*builtinFunctions[]) (char **) = {&exit, &chdir, &cd, &getenv, &setenv, &echo};
        int numCommands = sizeof(builtinCommands) / sizeof(char*);


        if(exit(args) == 1){
                return 1;
        }

        return launch(args);
}


int launch(char** args){
	pid_t pid, wpid;
	int status;
	
	pid = fork();
	if(pid == 0){   // Child Process
		execv(args[0], args);
		// If execv returns, it failed	
		perror("Child Process Error");
		exit(EXIT_FAILURE);
	}
	
	else if(pid<0){
		perror("Error forking");
	}

	else{  // Parent Process
		do{
			wpid = waitpid(pid, &status, WUNTRACED);  // Wait for process's status to change
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
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
	char** tokens = malloc(currentTokenBufferSize * sizeof(char*));  // Allocate for each token
	char* token;
	char* saveptr;

	if(!tokens){
		fprintf(stderr, "Token Allocation Error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok_r(line, TOKEN_DELIM, &saveptr);  // Splits up the tokens by spaces and other inputs
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
		token = strtok_r(NULL, TOKEN_DELIM, &saveptr);
	}
	tokens[position] = NULL;
	return tokens;
}
