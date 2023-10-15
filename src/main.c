#include "./headers/base.h"

void printPrompt(char *currentDirectory);
void receiveLine(char *userInput, int size);
int countArgs(char *userInput);
void splitInput(char **inputArgs, char *userInput);

int main(int argc, const char *argv[])
{
    __uint8_t check = 1;
    char *userInput = (char *)calloc(sizeof(char), INPUT_SIZE); // user text input

    __uint8_t pathElements = 1;
    char **binPaths = (char **)calloc(sizeof(char *), pathElements); // String array for path elements. starting with /bin
    binPaths[pathElements - 1] = strdup(BASE_PATH);                  // copying the first path element in the path elements array

    char **inputArgs = NULL; // input args buffer, initially empty
    __uint8_t argsNum;       // counting args number for each input

    char currentDirectory[PATH_MAX];
    getcwd(currentDirectory, sizeof(currentDirectory)); // getting working dir

    __uint8_t commandFound = 0;

    FILE *historyFile = fopen(".history", "wa"); // TODO: enable append
    if(historyFile == NULL) return 1;

    system("clear"); // just to clear the cli

    while (check)
    {
        printPrompt(currentDirectory);
        receiveLine(userInput, INPUT_SIZE);
        if (!strcmp(userInput, EXIT_COMMAND))
        {
            check = 0;
        }
        else
        {
            argsNum = countArgs(userInput);

            inputArgs = (char **)calloc(sizeof(char *), argsNum);

            splitInput(inputArgs, userInput);

            fprintf(historyFile, "%s\n", userInput);

            if (!strcmp(inputArgs[0], "cd"))
            {
                chdir(inputArgs[1]);
                getcwd(currentDirectory, sizeof(currentDirectory));
            }
            else if (!strcmp(inputArgs[0], "path"))
            {
                for (int i = 1; i < argsNum; i++)
                {
                    if (inputArgs[i][0] != '/')
                    {
                        printf("Error in arguments\n");
                        break;
                    }
                    else
                    {
                        binPaths[i] = strdup(inputArgs[i]);
                        pathElements++;
                    }
                }

                for (int i = 0; i < pathElements; i++)
                {
                    printf("%s\n", binPaths[i]);
                }
            }
            else if (!strcmp(inputArgs[0], "history"))
            {
            }
            else
            {
                if (inputArgs[0][0] == '/')
                {
                }
                else if (inputArgs[0][0] == '.')
                {
                }
                else
                {
                    for (int i = 0; i < pathElements && !commandFound; i++)
                    {
                        char *binPath = (char *)realloc(binPath, sizeof(char) * (strlen(binPaths[i]) + strlen(inputArgs[0])) + 2);
                        strcpy(binPath, binPaths[i]);
                        strcat(binPath, "/");
                        strcat(binPath, inputArgs[0]);
                        if (!access(binPath, X_OK))
                        {
                            commandFound = 1;
                            __pid_t res = fork();
                            if (res < 0)
                            {
                                printf("Couldn't start command >:(\n");
                            }
                            else if (!res)
                            {
                                execv(binPath, inputArgs);
                            }
                            else
                            {
                                int status;
                                waitpid(res, &status, 0);
                            }
                        }
                    }
                    if (!commandFound)
                    {
                        printf("Command not found\n");
                    }
                    else
                        commandFound = 0;
                }
            }
        }
    }

    free(userInput);

    fclose(historyFile);

    return 0;
}

void printPrompt(char *currentDirectory)
{
    printf(ANSI_COLOR_BLUE "<seashell> " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "$ ", currentDirectory);
}

void receiveLine(char *userInput, int size)
{
    fgets(userInput, size, stdin);
    __uint8_t length = strlen(userInput);
    for (int i = 0; i < length; i++)
    { // removing new line and eventually fix extra spaces for later parsing
        if (userInput[i] == '\n')
            userInput[i] = 0;
        else if (i < length - 1 && userInput[i] == ' ' && userInput[i + 1] == ' ')
        {
            userInput[i] = 0;
            break;
        }
    }
}

int countArgs(char *userInput)
{
    int counter = 0, check = 1;
    for (int i = 0; userInput[i] != 0 && check; i++)
    {
        if (userInput[i] == ' ')
        {
            counter++;
        }
    }
    return ++counter;
}

void splitInput(char **inputArgs, char *userInput)
{
    char *tok = userInput,
         *end = userInput;
    __uint8_t i = 0;

    while (tok != NULL)
    {
        strsep(&end, " ");
        inputArgs[i] = (char *)realloc(inputArgs[i], sizeof(char) * strlen(tok) + 1);
        strcpy(inputArgs[i++], tok);
        tok = end;
    }
}
