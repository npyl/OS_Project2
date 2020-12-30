#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>   // shm_open, mmap, munmap
#include <semaphore.h>

#include "b+-tree.h"

#define MIN_CHILDREN 1
#define MAX_CHILDREN 1000

// shared
static int *p = NULL;
static bptree_node* root = NULL;

int get_next_leaf_data(bptree_node* root) { 
    int sum = 0;
    for (int i = 0; i < root->keys_count; i++)
    {
        sum += root->keys[i];
    }
    return sum; 
}

void critical_region(int i)
{
    *p += get_next_leaf_data(root);
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
    int N       = get_number_of_children(argc, argv);
    pid_t* pid  = calloc(N, sizeof(pid_t));
    sem_t* sem  = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int child_status;

    // create a shared-between-processes semaphore (PSHARED must be 1)
    // Initialise to 1 so that all forks race to get it (probably the first one to be created will get it though)
    sem_init(sem, 1, 1);

    // make p shared
    p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // make b+-tree root shared
    root = mmap(NULL, sizeof(bptree_node), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // this initialisation is enough for our B+-Tree; kinda hacky but we are using mmap so we cannot malloc() twice!
    root->is_leaf = TRUE;

    // Insert N random values to our B+-Tree
    for (int i = 0; i < N; i++)
    {
        BOOL res = bptree_insert(root, rand() % 50);
        if (!res)
        {
            printf("Failed to insert elements at B+-Tree. Exiting...\n");
            _exit(EXIT_FAILURE);
        }
    }

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
            sem_wait(sem);
            critical_region(i);
            sem_post(sem);
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

    // (for debugging purposes)
    for (int i = 0; i < root->keys_count; i++)
    {
        printf("key[%d] = %d\n", i, root->keys[i]);
    }

    // sem_destroy or munmap??
    sem_destroy(sem);

    // deallocate shared p
    munmap(p, sizeof(int));

    // deallocate shared root
    munmap(root, sizeof(bptree_node));

    // deallocate whole B+-Tree
    bptree_destroy(root);

    return 0;
}