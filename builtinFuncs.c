// Name: Kyle Fritz
// File: builtinFuncs.c
// Date Created: 3/10/16
// Description: Provides the functionality of certain strings built into the shell

#include "utils.c"


int exitFunc(char** args){
    int numArgs = getNumArgs(args);

    if(numArgs>2){ // if more than two args, where one is exit
            printf("ERROR: Too many arguments\n");
            return 1;
    }

    if(args[1] == NULL || atoi(args[1]) == 0){ // exit is same as exit(0)
            exit(0);
    }
    else if(atoi(args[1]) == 1){
            exit(1);
    }
    else{
            printf("ERROR: Not a valid exit code\n");
            return 1;
    }
}

int chdirFunc(char** args){
//  printf("Go here -> %s\n", args[1]);
    if (args[1] == NULL || args[1][0] == '\n') {
        if(getenv("HOME")!=NULL){
                chdir(getenv("HOME"));       
        }
        else{
               fprintf(stderr, "ERROR: expected argument for changing directories\n");
       }
    }
    else {
    	args[1] = unescape(args[1], stderr);
            if (chdir(args[1]) != 0) {
           		perror("ERROR");
          	}
    	free(args[1]);
    }	
    return 1;
}

int cdFunc(char** args){
    return chdirFunc(args);
}

int getenvFunc(char** args){
    if(args[2] != NULL){
        fprintf(stderr, "ERROR: One argument expected\n");
        return 1;
    }

	if(getenv(args[1]) != NULL){
        printf(getenv(args[1]));
	}
    printf("\n");
    return 1;
}

int setenvFunc(char** args){
    /*** getting the key ***/
    int currentBufferSize = STR_BUFFER_SIZE; // start at size 30
    char* key = (char*) malloc(sizeof(char*)*currentBufferSize);
    int i = 0;
    while(args[1][i] != '='){ // get key, everything before '='
        if(i>=currentBufferSize){ // reallocate memory if need be
            currentBufferSize += currentBufferSize; // double the size every time
            key = realloc(key, currentBufferSize);
        }
        key[i] = args[1][i];  // update key
        i++;
    }


    int quote = 0;   // keep track of quotes; 1 -> one quote, 0 -> all quotes have been closed

    /*** getting the value ***/
    i++; // get to start of value
    currentBufferSize = STR_BUFFER_SIZE; // reset currentBufferSize
    char* value = (char*) malloc(sizeof(char*)*currentBufferSize);
    int j = 0;  // keep track of value with j
    int k = 1;  // go through all of args until after first full unescaped
    int maxK = getNumArgs(args) - 1; // max size of K, subtract one for setenv arg
    args[k] = unescape(args[k], stderr);
    bool boolean = true;

    while(boolean){ // get value, everything after '='
        if(j>=currentBufferSize){ // reallocate memory if need be
            currentBufferSize += currentBufferSize; // double the size every time
            value = realloc(value, currentBufferSize);
        }
        value[j] = args[1][i];  // update value

///////////////////// Attempt at solving quote problem //////////////////////
        if(j==0 && value[j] == '"'){
            quote = 1;
            printf("quote change!\n");
        }

        if(j>0){
            if(value[j] == '"' && value[j-1] != '\\'){ // if quote w/o a slash before it
                quote = !quote; // change quote value
                printf("quote change!\n");
            }
        }

        if(quote == 0 && args[k][i] == '\0'){
            //printf("in here\n");
            boolean = false;
        }
        else if(quote == 1 && args[k][i] == '\0'){
            i = -1; // reset i, about to i++ so needs to be -1
            printf("in here\n");
            k++;
            if(k >= maxK){ // get out
                boolean = false;
                break;
            }
            args[k] = unescape(args[k], stderr);

        }
/////////////////////////////////////////////////////////////////////////////

        i++;
        j++;
    }

    setenv(key, value, 1);

    free(key);
    free(value);
    return 1;
}

int echoFunc(char** args){ 
    int i = 1;
    
    while(args[i]!=NULL){
        args[i] = unescape(args[i], stderr); // unescaped version
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
    int j = 1;
    while(j<i){
        free(args[j]); // free everything
        j++;
    }
    return 1;
}
