#include "./headers/history.h"
#include "./headers/cli.h"
#include "./headers/config.h"

char *executeAndStoreResult(char *partly);

int main(int argc, const char *argv[])
{
    __uint8_t check = 1;                                        // looping in the shell
    char *userInput = (char *)calloc(sizeof(char), INPUT_SIZE); // user text input

    __uint8_t pathElements = 1;
    char **binPaths = (char **)calloc(sizeof(char *), pathElements); // String array for path elements. starting with /bin
    binPaths[pathElements - 1] = strdup(BASE_PATH);                  // copying the first path element in the path elements array

    char **inputArgs = NULL; // input args buffer, initially empty
    __uint8_t argsNum;       // counting args number for each input

    char currentDirectory[PATH_MAX];
    getcwd(currentDirectory, sizeof(currentDirectory)); // getting working dir

    __uint8_t commandFound = 0;

    char hostname[PATH_MAX];
    gethostname(hostname, PATH_MAX);

    FILE *historyFile;
    char historyLine[INPUT_SIZE + 30]; // history line buffer
    int historyLength;

    time_t t;
    struct tm tm;

    struct passwd *pw = getpwuid(getuid());
    char *historyPathname = (char *)calloc(sizeof(char), PATH_MAX);
    strcpy(historyPathname, pw->pw_dir);
    strcat(historyPathname, HISTORY_FILENAME); // defining history file path, inside the user's home directory

    // loading config file
    alias *aliases = NULL;
    int aliasCount = 0;
    char *configPath = (char *)calloc(sizeof(char), PATH_MAX);
    strcpy(configPath, pw->pw_dir);
    strcat(configPath, CONFIG_FILENAME);
    if (!loadAliases(configPath, &aliases, &aliasCount))
    {
        printf("Invalid config!\n");
        return -1;
    }

    // end loading config file

    // starting all the fancy things
    system("clear"); // just to clear the cli

    if (argc > 1)
    {
        if (!strcmp(argv[1], "-h"))
        {
            printHelp();
            return 0;
        }
    }

    if (!(argc > 1 && !strcmp(argv[1], "-q")))
    {
        printWelcome();
    }

    char receivedChar;

    __uint128_t historyNavigationPos = 0; // to track position in the history navigation

    while (check)
    {
        historyLength = 0;
        historyFile = fopen(historyPathname, "r");
        while (fgets(historyLine, INPUT_SIZE + 30, historyFile)) // counting history lines
        {
            historyLength++;
        }
        fclose(historyFile);
    newCommand: // maybe it can be removed, but i'm not sure if I want to do that (hihihi)
        printPrompt(currentDirectory, hostname);

        for (int j = 0; j < INPUT_SIZE; j++)
        {
            userInput[j] = 0;
        }

        system("stty raw -echo");
        int i = 0;
        do
        {
            historyFile = fopen(historyPathname, "r"); // to eventually consult the history
            receivedChar = getc(stdin);
            if (receivedChar == 127) // delete char
            {
                if (i > 0)
                {
                    if (i < strlen(userInput))
                    {
                        printf("\b");
                        int offset = strlen(userInput) - i;
                        for (int j = i - 1; j < strlen(userInput); j++)
                        {
                            userInput[j] = userInput[j + 1];
                            printf("%c", userInput[j]);
                        }
                        printf(" ");
                        for (int j = 0; j < offset + 1; j++)
                        {
                            printf("\b");
                        }
                        i--;
                    }
                    else
                    {
                        userInput[--i] = 0;
                        printf("\b \b");
                    }
                }
            }
            else if (receivedChar == 9)
            {
                char *result = executeAndStoreResult(userInput);
                if (result != NULL)
                {
                    if (strlen(userInput) > 0)
                    {
                        strcpy(userInput, result);
                        rewritePrompt(userInput, i);
                        for(int i = 0; i < strlen(userInput) - 1; i++){
                            printf("%c", userInput[i]);
                        }
                    }
                }
            }
            else if (receivedChar == 126)
            { // if del char is received
                for (int j = i; j < strlen(userInput); j++)
                {
                    userInput[j] = userInput[j + 1];
                }
                for (int j = i; j < strlen(userInput); j++)
                {
                    printf("%c", userInput[j]);
                }
                printf(" ");
                for (int j = 0; j < strlen(userInput) - i + 1; j++)
                {
                    printf("\b");
                }
            }
            else if (receivedChar == 3) // ctrl c
            {
                system("stty cooked echo");
                printf("\n");
                printf("To close the terminal, type 'exit'\n");
                goto newCommand;
            }
            else if (receivedChar == 12) // ctrl l
            {
                system("clear");
                printPrompt(currentDirectory, hostname);
            }
            else if (receivedChar == 27) // arrows
            {
                receivedChar = getchar(); // Read the next character
                if (receivedChar == '[')
                {
                    receivedChar = getchar(); // Read the arrow key character

                    if (receivedChar == 'A')
                    {
                        // Up Arrow
                        navigateHistory(&historyNavigationPos, historyFile, userInput, &i, historyLength, 1);
                    }
                    else if (receivedChar == 'B')
                    {
                        // Down Arrow
                        navigateHistory(&historyNavigationPos, historyFile, userInput, &i, historyLength, 0);
                    }
                    else if (receivedChar == 'C')
                    {
                        // Right Arrow
                        if (i < strlen(userInput))
                        {
                            printf("%c", userInput[i]);
                            i++;
                        }
                    }
                    else if (receivedChar == 'D')
                    {
                        // Left Arrow
                        if (i > 0)
                        {
                            i--;
                            printf("\b");
                        }
                        else
                            printf("\a");
                    }
                }
            }
            else if (receivedChar != 10 && receivedChar != 13)
            {
                for (int j = strlen(userInput) + 1; j > i; j--) // shifting chars for the newly inserted one
                {
                    userInput[j] = userInput[j - 1];
                }
                userInput[i] = receivedChar;
                int offset = strlen(userInput) - i; // calculating the offset between insertion position and last command char
                for (int j = i; j < strlen(userInput); j++)
                {
                    printf("%c", userInput[j]); // reprinting the rest of the command, after the new char
                }
                i++;
                if (offset > 1) // eventually repositioning cursor after the new char
                {
                    for (int j = 0; j < offset - 1; j++)
                    {
                        printf("\b");
                    }
                }
                fflush(stdout);
            }
            else
            {
                break;
                fclose(historyFile);
            }
        } while (1);

        printf("%c\n", 13);
        system("stty cooked echo");

        historyNavigationPos = 0; // resetting history navigation

        if (!strcmp(userInput, EXIT_COMMAND)) // if the exit command is received
        {
            check = 0;
        }
        else
        {
        logCommand:
            if (i > 0)
            {
                historyFile = fopen(historyPathname, "a");
                if (historyFile != NULL)
                {
                    t = time(NULL); // adding timestamp to the history
                    tm = *localtime(&t);
                    fprintf(historyFile, "%d-%02d-%02d %02d:%02d:%02d -> ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    fprintf(historyFile, "%s\n", userInput); // saving command in history
                    fclose(historyFile);
                }
                else
                {
                    historyFile = fopen(historyPathname, "w");
                    fclose(historyFile);
                    goto logCommand;
                }
            }

            if (inputArgs != NULL)
            { // if previous commands were run, freeing memory
                for (int i = 0; i < argsNum; i++)
                {
                    free(inputArgs[i]);
                }
                free(inputArgs);
            }

            argsNum = countArgs(userInput); // counting received arguments
            inputArgs = (char **)calloc(sizeof(char *), argsNum);

            splitInput(inputArgs, userInput);

            if (!strcmp(inputArgs[0], "cd"))
            {
                if (argsNum == 1)
                {
                    chdir(pw->pw_dir);
                }
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
                        pathElements++;
                        binPaths = (char **)realloc(binPaths, sizeof(char *) * pathElements);
                        binPaths[pathElements - 1] = strdup(inputArgs[i]);
                    }
                }

                for (int i = 0; i < pathElements; i++)
                {
                    printf("%s; ", binPaths[i]);
                }
                printf("\n");
            }
            else if (!strcmp(inputArgs[0], "history"))
            {
                if (argsNum > 1)
                {
                    if (!strcmp(inputArgs[1], "-c"))
                    {
                        historyFile = fopen(historyPathname, "w");
                        fclose(historyFile);
                    }
                }
                else
                {
                    historyFile = fopen(historyPathname, "r");
                    if (historyFile != NULL)
                    {
                        while (fgets(historyLine, INPUT_SIZE + 30, historyFile))
                        {
                            printf("%s", historyLine);
                        }
                    }
                    else
                    {
                        printf("No history file was found!\n");
                    }
                }
            }
            else if (!strcmp(inputArgs[0], "about"))
            {
                printWelcome();
            }
            else
            {
                if (strlen(userInput) > 0)
                {
                    if (inputArgs[0][0] == '/' || inputArgs[0][0] == '.') // if the command contains the absolute or relative path
                    {
                        if (!access(inputArgs[0], X_OK))
                        {
                            __pid_t res = fork();
                            if (res < 0)
                            {
                                printf("Couldn't start command >:(\n");
                            }
                            else if (!res)
                            {
                                execv(inputArgs[0], inputArgs);
                            }
                            else
                            {
                                int status;
                                waitpid(res, &status, 0);
                            }
                        }
                        else
                        {
                            printf("%s> Command not found\n", userInput);
                        }
                    }
                    else
                    {

                        char *realCommand = strdup(getAlias(aliases, aliasCount, inputArgs[0]));
                        char *realCommandBackup = strdup(realCommand);
                        char *anotherRealCommandBackup = strdup(realCommand);

                        realCommand = strtok(realCommand, " ");

                        for (int i = 0; i < pathElements && !commandFound; i++) // if only the command name, search in all paths
                        {
                            char *binPath = (char *)calloc(sizeof(char), (strlen(binPaths[i]) + strlen(realCommand) + 2)); // creating the asbolute path for the command
                            strcpy(binPath, binPaths[i]);
                            strcat(binPath, "/");
                            strcat(binPath, realCommand);

                            if (!access(binPath, X_OK))
                            {
                                commandFound = 1;
                                int newArgs = 0;
                                if (strcmp(realCommand, inputArgs[0]))
                                { // if the received command has an alias, also update the arguments

                                    realCommandBackup = strtok(realCommandBackup, " ");
                                    while (realCommandBackup != NULL)
                                    {
                                        realCommandBackup = strtok(NULL, " ");
                                        newArgs++;
                                    }
                                    char **tempOldArgs = (char **)calloc(sizeof(char *), argsNum);
                                    for (i = 0; i < argsNum; i++)
                                    {
                                        tempOldArgs[i] = strdup(inputArgs[i]);
                                    }
                                    inputArgs = (char **)realloc(inputArgs, sizeof(char *) * (newArgs + argsNum - 1));
                                    anotherRealCommandBackup = strtok(anotherRealCommandBackup, " ");

                                    int i = 0;
                                    while (anotherRealCommandBackup != NULL)
                                    {
                                        inputArgs[i++] = strdup(anotherRealCommandBackup);
                                        anotherRealCommandBackup = strtok(NULL, " ");
                                    }

                                    for (int i = 0; i < argsNum - 1; i++)
                                    {
                                        inputArgs[i + newArgs] = strdup(tempOldArgs[i + 1]);
                                    }

                                    for (i = 0; i < argsNum; i++)
                                    {
                                        free(tempOldArgs[i]);
                                    }
                                    free(tempOldArgs);
                                }

                                inputArgs = (char **)realloc(inputArgs, sizeof(char *) * (argsNum + newArgs + 1));
                                inputArgs[argsNum + newArgs] = NULL;
                                /*printf("%d", argsNum + newArgs);

                                printf("Command found at %s\n", binPath);
                                int i = 0;
                                while(inputArgs[i] != NULL){
                                    printf("%s ", inputArgs[i]);
                                    i++;
                                }
                                printf("\n");*/

                                __pid_t res = fork();
                                if (res < 0)
                                {
                                    printf("Couldn't start command >:(\n");
                                }
                                else if (res == 0)
                                {

                                    execv(binPath, inputArgs);
                                    exit(0);
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
                            printf("%s> Command not found\n", realCommand);
                        }
                        else
                            commandFound = 0;
                    }
                }
            }
        }
    }

    free(userInput);
    free(historyPathname);
    unloadAliases(aliases, aliasCount);

    for (int i = 0; i < pathElements; i++)
    {
        free(binPaths[i]);
    }
    free(binPaths);

    return 0;
}

char *executeAndStoreResult(char *partly)
{
    FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("ls -1", "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path) - 1, fp) != NULL)
    {
        char *result = strstr(path, partly);
        if (result != NULL)
        {
            pclose(fp);
            return strdup(path);
        }
    }

    /* close */
    pclose(fp);
    return NULL;
}