/*
    utils.c -- Various useful shell utility functions.

    Copyright (C) 2016 Lawrence Sebald
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

typedef int bool;
#define true 1
#define false 0

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h> // getuid(void)
#include <sys/types.h>
#include <pwd.h> // getpwuid(uid_t uid)
#define STR_BUFFER_SIZE 30
#define BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIM " "
#include "utils.h"


int checkForProfile(){
    struct passwd* pass = getpwuid(getuid()); // get the userid and password to get HOME dir
    char* homeDir = pass->pw_dir; // get HOME dir
    const char* profile = strcat(homeDir, "/.421sh_profile"); // concatenate for fopen

    FILE* file;
    if(file = fopen(profile, "r")) {
        return 1;
    }
    else{
        return 0;
    }
}

void writeToHistory(char* line){
    struct passwd* pass = getpwuid(getuid()); // get the userid and password to get HOME dir
    char* homeDir = pass->pw_dir; // get HOME dir
    const char* hist = strcat(homeDir, "/.421sh_history"); // concatenate for fopen

    FILE* file; // 
    file = fopen(hist, "a+"); // append to .421sh_history
    fputs(line, file); // append to end of file
    fputc('\n', file);

    fclose(file);
}

void printString(char* str){
    printf("Printing string... \n");
    int i = 0;
    while(str[i] != '\0'){
        printf("%c\n", str[i]);
        i++;
    }
}

void printArray(char** array){
    printf("Printing array... \n");
    int i = 0;
    while(array[i] != NULL){
        printf("%s\n", array[i]);
        i++;
    }
}

void printNumArgs(char** args){
    int numArgs = getNumArgs(args);
    printf("Number of argument given: %d\n", numArgs);
}

int getNumArgs(char** args){ // get total number of arguments
    int i = 0;
    while(args[i] != NULL){
        i++;
    }
    return i;
}


size_t count_spaces(const char *str) {
    size_t rv = 0;

    while(*str) {
        if(isspace(*str++))
            ++rv;
    }

    return rv;
}

void flush_input(FILE *fp) {
    int c;

    /* Read until we get an EOF character or a newline. */
    while(1) {
        c = fgetc(fp);

        if(c == '\n' || c == EOF)
            return;
    }
}

char *unescape(const char *str, FILE *errf) {
    size_t len = strlen(str), i;
    char *unesc, *rv;
    char cur;
    char quoted = 0;

    /* Allocate space for the new string. Since only potentially removing
       characters, it will be no larger than the original string... */
    if(!(rv = (char *)malloc(len + 1))) {
        fprintf(errf, "shell error: %s\n", strerror(errno));
        return NULL;
    }

    unesc = rv;

    /* Scan through the string... */
    while(*str) {
        cur = *str++;

        /* Is this the beginning of an escape sequence? */
        if(!quoted && cur == '\\') {
            cur = *str++;

            switch(cur) {
                case '\0':
                    fprintf(errf, "shell error: illegal escape sequence\n");
                    free(rv);
                    return NULL;

                case 'n':
                    *unesc++ = '\n';
                    continue;
                case 'a':
                    *unesc++ = '\a';
                    continue;
                case 'b':
                    *unesc++ = '\b';
                    continue;
                case 'r':
                    *unesc++ = '\r';
                    continue;
                case '\\':
                    *unesc++ = '\\';
                    continue;
                case 'f':
                    *unesc++ = '\f';
                    continue;
                case 'v':
                    *unesc++ = '\v';
                    continue;
                case '\'':
                    *unesc++ = '\'';
                    continue;
                case '"':
                    *unesc++ = '"';
                    continue;
                case '?':
                    *unesc++ = '?';
                    continue;
                case '*':
                    *unesc++ = '*';
                    continue;
                case '$':
                    *unesc++ = '$';
                    continue;
                case 't':
                    *unesc++ = '\t';
                    continue;
                case ' ':
                    *unesc++ = ' ';
                    continue;
                case '!':
                    *unesc++ = '!';
                    continue;

                /* Ugh... Octal. */
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                {
                    int tmp = (cur - '0') << 6;

                    cur = *str++;
                    if(cur < '0' || cur > '7') {
                        free(rv);
                        return NULL;
                    }

                    tmp |= (cur - '0') << 3;

                    cur = *str++;
                    if(cur < '0' || cur > '7') {
                        free(rv);
                        return NULL;
                    }

                    tmp |= (cur - '0');
                    *unesc++ = (char)tmp;
                    continue;
                }

                /* And, for more fun, hex! */
                case 'x':
                case 'X':
                {
                    int tmp;

                    cur = *str++;
                    if(cur >= '0' && cur <= '9')
                        tmp = (cur - '0') << 4;
                    else if(cur >= 'a' && cur <= 'f')
                        tmp = (cur - 'a' + 10) << 4;
                    else if(cur >= 'A' && cur <= 'F')
                        tmp = (cur - 'A' + 10) << 4;
                    else {
                        free(rv);
                        return NULL;
                    }

                    cur = *str++;
                    if(cur >= '0' && cur <= '9')
                        tmp |= (cur - '0');
                    else if(cur >= 'a' && cur <= 'f')
                        tmp |= (cur - 'a' + 10);
                    else if(cur >= 'A' && cur <= 'F')
                        tmp |= (cur - 'A' + 10);
                    else {
                        free(rv);
                        return NULL;
                    }

                    *unesc++ = (char)tmp;
                    continue;
                }

                default:
                    *unesc++ = cur;
            }

            continue;
        }
        /* Escape sequences in quoted strings are very limited... */
        else if(quoted && cur == '\\') {
            cur = *str++;

            if(!cur) {
                fprintf(errf, "shell error: invalid escape sequence\n");
                free(rv);
                return NULL;
            }

            if(cur != quoted)
                *unesc++ = '\\';
        }
        /* Is this the beginning of a quoted string? */
        else if(!quoted && (cur == '\'' || cur == '"')) {
            quoted = cur;
            continue;
        }
        else if(quoted && cur == quoted) {
            quoted = 0;
            continue;
        }

        /* If we get here, it's not part of an escape, so copy it directly. */
        *unesc++ = cur;
    }

    /* Did we terminate any quotes that we started? */
    if(quoted) {
        fprintf(errf, "shell error: unterminated quote\n");
        free(rv);
        return NULL;
    }

    *unesc = 0;

    return rv;
}

int first_unquoted_space(const char *str) {
    int pos = 0;
    const char *tmp = str;
    char quoted = 0, last = 0, cur;

    while(*tmp) {
        last = cur;
        cur = *tmp;

        if(last != '\\') {
            if(!quoted && (cur == '\'' || cur == '"'))
                quoted = cur;
            else if(quoted && cur == quoted)
                quoted = 0;
            if(!quoted && isspace(cur))
                return pos;
        }

        ++pos;
        ++tmp;
    }

    return -1;
}
