// Name: Kyle Fritz
// File: main.c
// Date Created: 2/20/16
// Description: main file for my own shell

#include "shellFunctions.c"

void shell(){ // regular shell
	char prompt = '$';
	char* line;
	char** args;
	int status;

	do{
		printf("%c ", prompt);
		line = readLine();
		writeToHistory(line);
		args = splitLine(line);
		status = execute(args);

		free(line);
		free(args);
	} while(status);	
}

void readScript(FILE* file){ // for scripts
	char prompt = '$';
	char* line;
	char** args;
	int status;

	do{
		printf("%c ", prompt);
		line = readLineScript(file);
		args = splitLine(line);
		status = execute(args);
		free(line);
		free(args);
	} while(status);
}

int main(int argc, char *argv[])
{
	FILE* profile;
	if(checkForProfile()){
		profile = fopen(argv[1], "r");
		readScript(profile);
		fclose(profile);
	}

	FILE* file;

	if(argv[1] == NULL){ // normal shell call
		shell();
	}
	
	else{
		if(file = fopen(argv[1], "r")) { // if file can be opened and read, use it
			readScript(file);
			fclose(file);
		}
		else{ // else, exit
			fprintf(stderr, "File Not Found\n");
		}
	}
}
