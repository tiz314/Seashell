#include "./headers/history.h"
#include "./headers/cli.h"
#include "./headers/base.h"

#include <stdio.h>
#include <stdlib.h>

void navigateHistory(__uint128_t *historyNavigationPos, FILE *historyFile, char *userInput, int *promptCursor, int historyLength, __uint8_t direction) // navigating up and down through history, updating the variables
{
    char historyLine[INPUT_SIZE + 30]; // receiving history lines from the history file
    int offset = 0, counter = 0;       // offset from the last command
    if (direction)
    { // up
        *historyNavigationPos = *historyNavigationPos < historyLength ? *historyNavigationPos + 1 : *historyNavigationPos;
    }
    else
    {
        *historyNavigationPos = *historyNavigationPos > 1 ? *historyNavigationPos - 1 : *historyNavigationPos;
    }
    rewind(historyFile);
    offset = historyLength - *historyNavigationPos;                                // calculating the offset for the history navigation
    while (fgets(historyLine, INPUT_SIZE + 30, historyFile) && counter++ < offset) // reaching the asked command
        ;                                                                          // navigating through history

    counter = 0;    // resetting the counter, now counting where the command begins
    int spaces = 0; // counting spaces to reach the command in historyLine
    while (historyLine[counter] && spaces < 3)
    {
        if (historyLine[counter] == ' ')
            spaces++;
        counter++;
    }
    clearPrompt(userInput, *promptCursor);  // rewriting the prompt
    strcpy(userInput, historyLine + counter); // copying the command in the user input
    userInput[strlen(userInput) - 1] = 0;     // removing the new line
    *promptCursor = strlen(userInput);
    printf("%s", userInput); // reprinting the command
}