#include "./base.h"

void loadHistory(char **historyLines, char *historyPathname);

void navigateHistory(__uint128_t *historyNavigationPos, FILE *historyFile, char *userInput, int *promptCursor, int historyLength, __uint8_t direction);