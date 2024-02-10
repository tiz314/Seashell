#include "./headers/cli.h"

#include <stdio.h>

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

void rewritePrompt(char *userInput, int cursorPosition)
{
    for (int j = 0; j < cursorPosition; j++)
    {
        printf("\b"); // clearing the prompt from the old command
    }
    for (int j = 0; j < strlen(userInput); j++)
    {
        printf(" "); // clearing the prompt from the old command
    }
    for (int j = 0; j < strlen(userInput); j++)
    {
        printf("\b"); // clearing the prompt from the old command
    }
}