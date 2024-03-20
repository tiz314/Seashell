#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
    pid_t pid = fork(); // create a new process

    if (pid == 0) { // child process
        char *argv[] = { "nc", "spg.challs.cyberchallenge.it", "9600", NULL }; // replace with your nc command
        execv("/bin/nc", argv); // replace with the path to your nc executable
    } else if (pid > 0) { // parent process
        // parent code here
    } else { // fork failed
        printf("fork() failed!\n");
        return 1;
    }

    return 0;
}