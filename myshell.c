#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define DEBUG printf("Debug\n")
#define BUFFER_SIZE 1024

void parse_command(char *buf, char **args, char **outfile){
    char *token = strtok(buf, " ");
    int i = 0;
    *outfile = NULL;

    while (token){
        if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                *outfile = token;
            }
            break;
        }
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

int main()
{
    char buf[BUFFER_SIZE];
    char **cmd;
    char *outfile;

    while (1){
        printf("myshell> ");

        if (fgets(buf, BUFFER_SIZE, stdin) == NULL) {
            perror("fgets error");
            continue;
        }

        buf[strcspn(buf, "\n")] = 0;

        if (strcmp(buf, "exit") == 0){
            break;
        }

        parse_command(buf, cmd, &outfile);

        pid_t pid = fork();
        if (pid < 0){
            fprintf(stderr, "Fork Error\n");
            return -1;
        }

        if (pid == 0){
            if (outfile){
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (fd < 0) {
                    perror("open error");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(cmd[0], cmd);
            perror("execvp error");
            exit(EXIT_FAILURE);
        } else {
            int ret;
            waitpid(pid, &ret, 0);
        }
    }

    return 0;
}
