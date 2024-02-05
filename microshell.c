#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int error(char *error)
{
    int i = 0;
    while (error[i++])
        write(2, &error[i], 1);
    return i;
}

int cd(int i, char **argv)
{
    if (i != 2)
        error("error: not enought params");
    else if (chdir(argv[1]) ==  -1)
        error("error: cannot change dir");
    return 0;
}

int exec(int i, char **argv, char **envp)
{
    int status;
    int fd[2];
    int has_pipe = argv[i] && !strcmp(argv[i], "|");

    if (has_pipe && pipe(fd) == -1)
        return error("error: fatal\n");
    
    int pid = fork();
    if (!pid)
    {
        argv[i] = 0;
        if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
            return error("error: fatal\n");
        execve(*argv, argv, envp);
        return error("error: cannot execute: "), error(*argv), error("\n");
    }
    waitpid(pid, &status, 0);
    if (has_pipe && (dup2(fd[0], 1) == -1 || close(fd[0]) == -1 || close(fd[1] == -1)))
        return error("error: fatal");
    return WIFEXITED(status), WEXITSTATUS(status);
}

int main(int argc, char **argv, char **envp)
{
    int i = 0;
    int status = 0;

    if (argc > 1)
    {
        while (argv[i] && argv[++i])
        {
            argv += 1;
            i = 0;
            while (argv[i] && (strcmp(argv[i], "|") || strcmp(argv[i], ";")))
                i++;
            if (!strcmp(*argv, "cd"))
                status += cd(i, argv);
            else if (i)
                status += exec(i, argv, envp);
        }
    }
    return status;
}