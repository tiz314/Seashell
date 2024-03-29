#include "./base.h"

void printWelcome();
void printHelp();

void printPrompt(char *currentDirectory, char *hostname);
void receiveLine(char *userInput, int size);
int countArgs(char *userInput);
void splitInput(char **inputArgs, char *userInput);

void addCharInWord(char *userInput, int i, char new);

void clearPrompt(char *userInput, int cursorPosition);
void fillPrompt(char *userInput, int *cursorPosition);