#include <stdio.h>
#include <string.h>

#include <syscall.h>
#include <stdioe.h>

#include <stdlib.h>

int mshell(char *user) __attribute__((section(".mshell")));

/* User callable shell program */
int mshell(char *user) {
    FILE_P file;
    char directive[100], directive_copy[100], context[200], path_temp[200];
    char *token = NULL, *ptr, delim[] = " ", delim2[] = "/";
    int file_id, c = 0;

    char COM[5][11];

    strcpy(COM[0], "cd");
    strcpy(COM[1], "..");
    strcpy(COM[2], "ls");

    directive[0] = '\0';
    strcpy(context, "/");

    while(1) {
        printf_P("%s:%s$ ", user, context);
        egets(directive);

        token = strtok(directive, delim);

        if(!strcmp(COM[0], token) && token != NULL) {
            token = strtok(NULL, delim);
            if (token == NULL) continue;

            if (!strcmp(COM[1], token) && strlen(context) > 1) {
                c = 0;

                ptr = strrchr(context, delim2[0]);
                ptr--;
                for (ptr; ptr >= context; ptr--) {
                    c++;
                    if (*ptr == '/') break;               
                }

                context[strlen(context) - c] = '\0';
                continue;
            }

            strcpy(path_temp, context);
            strcat(path_temp, token);
            strcat(path_temp, delim2);

            file_id = __ffind_P(path_temp);
            if (file_id == -1) {
                printf_P("'%s' is not a directory or doesn't exist.\n\r", token);
                continue;
            }

            __fstat_P(file_id, &file);
            if (file.type != F_DIR) {
                printf_P("'%s' is not a directory or doesn't exist.\n\r", token);
                continue;
            }

            strcpy(context, path_temp);

        } else if (!strcmp(COM[2], token)) {
            (void) __flist_P(context);
        } else {
            /* User perhaps wants to start a program */
            file_id = __ffind_P(token);
            __fstat_P(file_id, &file);

            if (file.type == F_PXE) {
                __call(file_id);
            } else {
                printf_P("'%s' is neither a command or an executable file.\n\r", token);
                continue;
            }
        }

        printf_P("\n\r");
    }

    return 0;
}