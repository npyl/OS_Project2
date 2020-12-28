#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>   // shm_open, mmap, munmap

#define MIN_CHILDREN 1
#define MAX_CHILDREN 1000

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
    int N = get_number_of_children(argc, argv);

    for (int i = 0; i < N; i++)
    {

    }

    return 0;
}