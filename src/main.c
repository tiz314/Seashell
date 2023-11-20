#include "./headers/base.h"

void printWelcome();
void printPrompt(char *currentDirectory, char *hostname);
void receiveLine(char *userInput, int size);
int countArgs(char *userInput);
void splitInput(char **inputArgs, char *userInput);

void loadHistory(char **historyLines, char **historyPathname);

void addCharInWord(char *userInput, int i, char new);

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

    char hostname[PATH_MAX];
    gethostname(hostname, PATH_MAX);

    FILE *historyFile;

    time_t t = time(NULL); // adding timestamp to the history
    struct tm tm = *localtime(&t);

    struct passwd *pw = getpwuid(getuid());
    char *historyPathname = (char *)calloc(sizeof(char), PATH_MAX);
    strcpy(historyPathname, pw->pw_dir);
    strcat(historyPathname, HISTORY_FILENAME); // defining history file path

    system("clear"); // just to clear the cli
    if (!(argc > 1 && !strcmp(argv[1], "-q"))){
        printWelcome();
    }

    char receivedChar;

    char **historyLines;
    __uint128_t historyNavigationPos; // to track position in the history navigation

    while (check)
    {
    newCommand:
        printPrompt(currentDirectory, hostname);

        for (int j = 0; j < strlen(userInput); j++)
        {
            userInput[j] = 0;
        }

        system("stty raw -echo");
        int i = 0;
        do
        {
            receivedChar = getc(stdin);
            if (receivedChar == 127) // delete char
            {
                if (i > 0)
                {
                    if (i < strlen(userInput))
                    {
                        int offset = strlen(userInput) - i - 1;
                        for (int j = i - 1; j < strlen(userInput); j++)
                        {
                            userInput[j] = userInput[j + 1];
                            printf("%c", userInput[j]);
                        }
                        printf(" ");
                        for (int j = 0; j < offset; j++)
                        {
                            printf("\b");
                        }
                    }
                    else
                    {
                        userInput[--i] = 0;
                        printf("\b \b");
                    }
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
                    }
                    else if (receivedChar == 'B')
                    {
                        // Down Arrow
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
                break;
        } while (1);

        userInput[i] = 0;
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
            historyFile = fopen(historyPathname, "a");
            if (historyFile != NULL)
            {
                fprintf(historyFile, "%d-%02d-%02d %02d:%02d:%02d -> ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                fprintf(historyFile, "%s\n", userInput); // saving command in history
                fclose(historyFile);
                loadHistory(historyLines, historyPathname);
            }
            else
            {
                historyFile = fopen(historyPathname, "w");
                fclose(historyFile);
                goto logCommand;
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
                FILE *historyFileRead = fopen(historyPathname, "r");
                if (historyFileRead != NULL)
                {
                    char historyLine[INPUT_SIZE];
                    while (fgets(historyLine, INPUT_SIZE, historyFileRead))
                    {
                        printf("%s", historyLine);
                    }
                }
                else
                {
                    printf("No history file was found!\n");
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
                    if (inputArgs[0][0] == '/' || inputArgs[0][0] == '.')
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

                        for (int i = 0; i < pathElements && !commandFound; i++)
                        {

                            char *binPath = (char *)calloc(sizeof(char), (strlen(binPaths[i]) + strlen(inputArgs[0]) + 2));
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
                            printf("%s> Command not found\n", userInput);
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

    for (int i = 0; i < pathElements; i++)
    {
        free(binPaths[i]);
    }
    free(binPaths);

    return 0;
}

void printPrompt(char *currentDirectory, char *hostname)
{
    char username[PATH_MAX];
    getlogin_r(username, PATH_MAX);

    printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_BLUE "%s " ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "$ ", username, hostname, currentDirectory);
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

void addCharInWord(char *userInput, int i, char new)
{
    for (int j = strlen(userInput); j > i; j--)
    {
        userInput[j] = userInput[j - 1];
    }
    userInput[i] = new;
}

void printWelcome()
{
    /*
       /\
      {.-}
     ;_.-'\
    {    _.}_
     \.-' /  `,
      \  |    /
       \ |  ,/
        \|_/
    */

    printf("\n\n" ANSI_COLOR_CYAN "   /\\\n");
    printf("  {.-}\n");
    printf(" ;_.-'\\\n");
    printf("{    _.}_     " ANSI_COLOR_RESET "Sea(C)shell\n");
    printf(ANSI_COLOR_CYAN " \\.-' /  `,  \n");
    printf(ANSI_COLOR_CYAN "  \\  |    /   " ANSI_COLOR_RESET "Just a Bash Shell emulator\n");
    printf(ANSI_COLOR_CYAN "   \\ |  ,/    " ANSI_COLOR_RESET "Made with <3 by Tiz314\n");
    printf(ANSI_COLOR_CYAN "    \\|_/      " ANSI_COLOR_RESET "https://github.com/tiz314/seashell\n\n\n");
}

void loadHistory(char **historyLines, char **historyPathname)
{
    FILE *historyFileRead = fopen(historyPathname, "r");
    __uint128_t historyLinesCounter = 0;
    if (historyFileRead != NULL)
    {
        char historyLine[INPUT_SIZE];
        while (fgets(historyLine, INPUT_SIZE, historyFileRead))
        {
            historyLinesCounter++;
        }
        printf("");
    }
    else
    {
        printf("No history file was found!\n");
    }
}