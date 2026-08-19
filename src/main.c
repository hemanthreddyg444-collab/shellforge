#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"

int main(void)
{
    /* Display welcome banner */
    printf("=====================================\n");
    printf("             Shellforge\n");
    printf("     A Unix Style Shell in C\n");
    printf("=====================================\n");

    token_list_t tokens;
    pipeline_t pipeline;

    char *line;

    while (1)
    {
        /* Display shell prompt */
        line = readline("shellforge$ ");

        /* Ctrl+D */
        if (line == NULL)
        {
            printf("\nGoodbye!\n");
            break;
        }

        /* Ignore empty input */
        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        /* Exit command */
        if (strcmp(line, "exit") == 0)
        {
            free(line);
            printf("Exiting...\n");
            break;
        }

        /* Add command to history */
        add_history(line);

        /*
         * Initialize token list before lexing.
         * This depends on your token_list_t structure.
         */
        tokens.count = 0;

        /* Lexical analysis */
        lexer(line, &tokens);

        /* Display tokens */
        token_print(&tokens);

        /* Parsing */
        if (parser(&tokens, &pipeline))
        {
            /* Expand environment variables */
            expand_variables(&pipeline);

            /* Display parsed pipeline */
            pipeline_print(&pipeline);
        }
        else
        {
            printf("Parser error\n");
        }

        /* Free input line */
        free(line);
    }

    return 0;
}
