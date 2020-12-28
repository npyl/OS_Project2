#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>   // shm_open, mmap, munmap

#define MIN_CHILDREN 1
#define MAX_CHILDREN 1000

void critical_region(int i)
{
    *p += i;
}

int get_number_of_children(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("You must provide a number of child processes to create..Please use ./<executable> <number_of_children>\n");
		exit(EXIT_FAILURE);
	}

	if(argc != 2)
	{
		printf("Invalid number of arguments given .. You only need to give one argument\n");
		exit(EXIT_FAILURE);
	}
	int children = atoi(argv[1]);

	if(children < MIN_CHILDREN || children > MAX_CHILDREN)
	{
		printf("Invalid children processes number\n");
		exit(EXIT_FAILURE);
	}

	return children;
}

int main(int argc, char** argv)
{
    int N           = get_number_of_children(argc, argv);
    pid_t* pid      = calloc(N, sizeof(pid_t));
    int *p          = NULL;

    /*
     * Create N children.
     */
    for (int i = 0; i < N; i++)
    {
        pid[i] = fork();

        if(pid[i] == 0)
        {
            /*
             * As a child, I will try to access my critical region
             * and then I will terminate normally using _exit(0).
             */
            critical_region(i);
            _exit(0);
        }
    }

    /*
     * Once the loop above has created N children, we *must*
     * wait for them to terminate using waitpid().  Otherwise
     * they will become zombie-processes.  After waitpid() returns
     * we also check their exit status to make sure they terminated normally.
     */
    for(int i = 0; i < N; i++)
    {
        pid_t wpid = waitpid(pid[i], &child_status, 0);

        if(WIFEXITED(child_status))
        {
            printf("Child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
        }
        else
        {
            printf("Child %d terminated abnormally\n", wpid);
        }
    }

    // Once every child has exited, print p
    printf("p = %d\n", *p);

    return 0;
}