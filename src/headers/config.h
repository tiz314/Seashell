typedef struct alias{
    char *command;
    char *alias;
} alias;

int loadAliases(char *configPath, alias **aliases, int *aliasCount);
char *getAlias(alias *aliases, int aliasCount, char *command);
void unloadAliases(alias *aliases, int aliasCount);