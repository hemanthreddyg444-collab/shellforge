#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "history.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "builtin.h"
#include "executor.h"

int main(void)
{
    /* Welcome banner */
    printf("=====================================\n");
    printf("             Shellforge\n");
    printf("     A Unix Style Shell written in C\n");
    printf("=====================================\n");

    /* Initialize readline history */
    using_history();

    char *line;

    while (1)
    {
        /* Read command from user */
        line = readline("shellforge$ ");

        /* Ctrl+D / EOF */
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

        /* Built-in history command */
        if (strcmp(line, "history") == 0)
        {
            print_history();
            free(line);
            continue;
        }

        /* Add command to history */
        add_history(line);

        /* Tokenization */
        token_list_t tokens;

        lexer(line, &tokens);

        /*
         * Parse tokens into a pipeline.
         * Only continue if parsing is successful.
         */
        pipeline_t pipeline;

        if (!parser(&tokens, &pipeline))
        {
            fprintf(stderr, "Shellforge: syntax error\n");
            free(line);
            continue;
        }

        /* Expand environment variables */
        expand_variables(&pipeline);

        /*
         * Check for the exit command.
         */
        if (pipeline.command_count == 1 &&
            pipeline.commands[0].argc > 0 &&
            pipeline.commands[0].argv[0] != NULL &&
            strcmp(pipeline.commands[0].argv[0], "exit") == 0)
        {
            free(line);
            break;
        }

        /* Execute command / pipeline */
        execute_pipeline(&pipeline);

        /* Free input line */
        free(line);
    }

    return 0;
}

