#include "./base.h"

typedef struct
{
    __uint8_t argsNum;
    __uint8_t specialKeys;
    __uint128_t keysPosition[10];
    __uint8_t keysType[10];
    __uint128_t lastIndexExecuted;
} analyzed;

void printWelcome();
void printHelp();

void printPrompt(char *currentDirectory, char *hostname);
void receiveLine(char *userInput, int size);
analyzed countArgs(analyzed inputAnalysis, char *userInput);
analyzed findSpecialKeys(char **inputArgs, analyzed inputAnalysis);
void splitInput(char **inputArgs, char *userInput);

void addCharInWord(char *userInput, int i, char new);

void clearPrompt(char *userInput, int cursorPosition);
void fillPrompt(char *userInput, int *cursorPosition);