#include "./headers/config.h"
#include "./headers/base.h"
#include <stdlib.h>


FILE *openConfigFile(char *configPath);

int loadAliases(char *configPath, alias **aliases, int *aliasCount)
{
    FILE *configFile = openConfigFile(configPath);

    char line[INPUT_SIZE];
    while (fgets(line, INPUT_SIZE, configFile))
    {
        char intro[6];
        strncpy(intro, line, 5);
        if (!strcmp(intro, "alias"))
        {
            *aliasCount = *aliasCount + 1;
        }
    }
    rewind(configFile);
    *aliases = (alias *)malloc(sizeof(alias) * *aliasCount);
    int counter = 0; 
    while (fgets(line, INPUT_SIZE, configFile))
    {
        char intro[6];
        strncpy(intro, line, 5);
        if (!strcmp(intro, "alias"))
        {
            char *lineBackup = strdup(line);

            char *alias = strtok(line, " ");
            alias = strtok(NULL, " "); // getting the command
            char *command = strtok(lineBackup, "=");
            command = strtok(NULL, "="); // getting the alias
            if(command[strlen(command) - 1] == '\n')
                command[strlen(command) - 1] = 0;
            if(alias[strlen(alias) - 1] == '\n')
                alias[strlen(alias) - 1] = 0; // removing the new line sometimes present
            (*aliases + counter)->command = strdup(command);
            (*aliases + counter)->alias = strdup(alias);
            counter++;
        }
    }

    fclose(configFile);

    return 1;
}

FILE *openConfigFile(char *configPath)
{
    FILE *configFile = fopen(configPath, "r");
    if (configFile == NULL)
    {
        while (configFile == NULL)
        {
            configFile = fopen(configPath, "w");
            fclose(configFile);
        }
        configFile = fopen(configPath, "r");
    }
    return configFile;
}

char *getAlias(alias *aliases, int aliasCount, char *command)
{
    for (int i = 0; i < aliasCount; i++)
    {
        if (!strcmp(aliases[i].alias, command))
        {
            return aliases[i].command;
        }
    }
    return command;
}

void unloadAliases(alias *aliases, int aliasCount)
{
    for (int i = 0; i < aliasCount; i++)
    {
        free(aliases[i].command);
        free(aliases[i].alias);
    }
    free(aliases);
}