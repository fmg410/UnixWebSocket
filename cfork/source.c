#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int makePerson(const char* name, const char* job, int time)
{
    int pid = fork();
    if(!pid)
    {
        printf("%d: %s is %s\n", getpid(), name, job);
        sleep(time);
        printf("%d: %s finished %s\n", getpid(), name, job);
        exit(0);
    }
    return pid;
}

void makeTeam(int offset, int delay, int _pids[3])
{
    int pids[3];
    pids[0] = makePerson("leader", "leading", 6 + offset);
    pids[1] = makePerson("builder", "building", 3 + offset);
    pids[2] = makePerson("helper", "helping", 4 + offset);

    sleep(delay);
    int exitStatus = 0;
    int whoFinished = 0;

    for(int i = 0; i < 3; i++)
    {
        whoFinished = wait(&exitStatus);
        printf("%d: %d finished its job with exit status: %d\n", getpid(), whoFinished, exitStatus);
    }
    memmove(_pids, pids, sizeof(pids));
}

int main()
{
    int pid = fork();

    if(!pid)
    {
        printf("Making team 1\n");
        int pidsTeam1[3];
        makeTeam(0, 0, pidsTeam1);

        printf("Making team 2\n");
        int pidsTeam2[3];
        makeTeam(4, 12, pidsTeam2);

        char buffer[3][50];
        snprintf(buffer[0], 49, "%d", pidsTeam1[0]);
        snprintf(buffer[1], 49, "%d", pidsTeam1[1]);
        snprintf(buffer[2], 49, "%d", pidsTeam1[2]);

        char *arr[] = {"team1.txt", buffer[0], buffer[1], buffer[2], NULL};
        execv("save", arr);
        return 0;
    }

    int exitStatus;
    int childPid = wait(&exitStatus);

    printf("Parent pid is %d, child pid from fork is %d, child pid from wait is %d, child exit status is %d\n", getpid(), pid, childPid, exitStatus);
    return 0;
}

