#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define INPUT_SIZE 4000
#define EXIT_COMMAND "exit"
#define BIN_PATH "/bin"

void printPrompt();
void receiveLine(char *userInput, int size);
int countArgs(char *userInput);

int main(int argc, const char *argv[])
{

    __uint8_t check = 1;
    char *userInput = (char *)calloc(sizeof(char), INPUT_SIZE);
    char *binPath = (char *)calloc(sizeof(char), INPUT_SIZE);

    char **inputArgs = NULL;

    strcpy(binPath, BIN_PATH);

    while (check)
    {
        printPrompt();
        receiveLine(userInput, INPUT_SIZE);
        if (!strcmp(userInput, EXIT_COMMAND))
        {
            check = 0;
        }
        else
        {

            char *tok = userInput, *end = userInput;
            while (tok != NULL)
            {
                strsep(&end, " ");
                puts(tok);
                tok = end;
            }
        }
    }

    free(userInput);
    free(binPath);
    
    return 0;
}

void printPrompt()
{
    printf(ANSI_COLOR_GREEN "Seashell$ " ANSI_COLOR_RESET);
}

void receiveLine(char *userInput, int size)
{
    fgets(userInput, size, stdin);
    userInput[strlen(userInput) - 1] = 0;
}

int countArgs(char *userInput)
{
    int counter = 0;
    for (int i = 0; userInput[i] != 0; i++)
    {
        if (userInput[i] == ' ')
            counter++;
    }
    return ++counter;
}