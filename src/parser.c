#include <stdio.h>
#include <string.h>

#include "parser.h"

/* Initialize one command */
static void command_init(command_t *cmd)
{
    cmd->argc = 0;

    cmd->input[0] = '\0';
    cmd->output[0] = '\0';

    cmd->append = 0;
    cmd->background = 0;

    for (int i = 0; i < MAX_ARGS; i++)
        cmd->argv[i] = NULL;
}

/* Parse tokens into a pipeline */
int parser(token_list_t *tokens, pipeline_t *pipeline)
{
    if (tokens == NULL || pipeline == NULL)
        return 0;

    pipeline->command_count = 1;

    int current = 0;

    command_init(&pipeline->commands[current]);

    for (int i = 0; i < tokens->count; i++)
    {
        token_t *t = &tokens->tokens[i];

        switch (t->type)
        {
            /* Normal command/argument */
            case TOKEN_WORD:

                if (pipeline->commands[current].argc >= MAX_ARGS - 1)
                {
                    printf("Parser Error: too many arguments.\n");
                    return 0;
                }

                pipeline->commands[current]
                    .argv[pipeline->commands[current].argc++] = t->text;

                break;


            /* Input redirection: < file */
            case TOKEN_INPUT:

                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Parser Error: filename expected after <\n");
                    return 0;
                }

                strncpy(pipeline->commands[current].input,
                        tokens->tokens[++i].text,
                        MAX_FILENAME - 1);

                pipeline->commands[current]
                    .input[MAX_FILENAME - 1] = '\0';

                break;


            /* Output redirection: > file */
            case TOKEN_OUTPUT:

                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Parser Error: filename expected after >\n");
                    return 0;
                }

                strncpy(pipeline->commands[current].output,
                        tokens->tokens[++i].text,
                        MAX_FILENAME - 1);

                pipeline->commands[current]
                    .output[MAX_FILENAME - 1] = '\0';

                pipeline->commands[current].append = 0;

                break;


            /* Append output: >> file */
            case TOKEN_APPEND:

                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Parser Error: filename expected after >>\n");
                    return 0;
                }

                strncpy(pipeline->commands[current].output,
                        tokens->tokens[++i].text,
                        MAX_FILENAME - 1);

                pipeline->commands[current]
                    .output[MAX_FILENAME - 1] = '\0';

                pipeline->commands[current].append = 1;

                break;


            /* Background execution: & */
            case TOKEN_BACKGROUND:

                pipeline->commands[current].background = 1;

                break;


            /* Pipe: | */
            case TOKEN_PIPE:

                if (pipeline->commands[current].argc == 0)
                {
                    printf("Parser Error: empty command before pipe.\n");
                    return 0;
                }

                /* NULL terminate argv */
                pipeline->commands[current]
                    .argv[pipeline->commands[current].argc] = NULL;

                current++;

                if (current >= MAX_COMMANDS)
                {
                    printf("Parser Error: too many commands in pipeline.\n");
                    return 0;
                }

                command_init(&pipeline->commands[current]);

                pipeline->command_count++;

                break;


            /* End of command */
            case TOKEN_END:

                break;


            default:

                printf("Parser Error: unknown token.\n");
                return 0;
        }
    }

    /* NULL terminate final command's argv */
    pipeline->commands[current]
        .argv[pipeline->commands[current].argc] = NULL;

    return 1;
}


/* Print parsed pipeline */
void pipeline_print(const pipeline_t *pipeline)
{
    if (pipeline == NULL)
        return;

    printf("\n========== PIPELINE ==========\n");

    for (int i = 0; i < pipeline->command_count; i++)
    {
        const command_t *cmd = &pipeline->commands[i];

        printf("\nCommand %d\n", i + 1);
        printf("-----------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < cmd->argc; j++)
        {
            printf("argv[%d] = %s\n", j, cmd->argv[j]);
        }

        printf("Input      : %s\n",
               strlen(cmd->input) > 0 ? cmd->input : "None");

        printf("Output     : %s\n",
               strlen(cmd->output) > 0 ? cmd->output : "None");

        printf("Append     : %s\n",
               cmd->append ? "Yes" : "No");

        printf("Background : %s\n",
               cmd->background ? "Yes" : "No");
    }

    printf("==============================\n");
}
