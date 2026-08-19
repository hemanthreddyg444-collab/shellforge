#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expand.h"

/*
 * Expand environment variables in command arguments.
 *
 * Example:
 *     echo $HOME
 *
 * becomes:
 *     echo /home/hemanth
 */
void expand_variables(pipeline_t *pipeline)
{
    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->command_count; i++)
    {
        command_t *cmd = &pipeline->commands[i];

        if (cmd == NULL)
            continue;

        for (int j = 0; j < cmd->argc; j++)
        {
            char *arg = cmd->argv[j];

            /* Ignore NULL arguments */
            if (arg == NULL)
                continue;

            /* Check whether argument starts with '$' */
            if (arg[0] == '$')
            {
                /* "$" alone is not a valid variable name */
                if (arg[1] == '\0')
                {
                    cmd->argv[j] = "";
                    continue;
                }

                /* Skip '$' and get variable name */
                char *name = arg + 1;

                /* Get value from environment */
                char *value = getenv(name);

                if (value != NULL)
                {
                    cmd->argv[j] = value;
                }
                else
                {
                    /*
                     * Undefined environment variables
                     * are replaced with an empty string.
                     */
                    cmd->argv[j] = "";
                }
            }
        }

        /* Ensure argv is NULL terminated */
        if (cmd->argc < MAX_ARGS)
            cmd->argv[cmd->argc] = NULL;
    }
}

