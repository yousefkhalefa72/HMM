#include <stdio.h>
#include "HMM.h"

#define MAX_ALLOCS 1000000
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define getInt(x, ...) atoi(x)
extern char end, edata, etext;

#if 0
int main(int argc, char *argv[])
{
	char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, j;
    printf("etext = %p, edata=%p, end=%p, initial program break=%p\n", &etext, &edata, &end, (char*)sbrk(0));

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        printf("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
        exit(1);
    }

    numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");
    if (numAllocs > MAX_ALLOCS) 
	{
        printf("num-allocs > %d\n", MAX_ALLOCS);
        exit(1);
    }

    blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

    freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
    freeMin =  (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
    freeMax =  (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

    if (freeMax > numAllocs) 
	{
        printf("free-max > num-allocs\n");
        exit(1);
    }

    printf("Initial program break:          %10p\n", sbrk(0));

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for (j = 0; j < numAllocs; j++) 
	{
        ptr[j] = malloc(blockSize);
        if (ptr[j] == NULL) 
		{
            printf("malloc returned null\n");
            exit(1);
        }
    }
    how_are_heap();

    printf("Program break is now:           %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n",freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j += freeStep)
        free(ptr[j]);

	printf("\e[1;34mdestorying heap:\e[0m ");
	DestroyHeap();
	printf("\n");
	
    printf("After free(), program break is: %10p\n", sbrk(0));
    how_are_heap();

    return 0;
}
#endif

#if 0
#define NUM_ALLOCS 10000
#define MAX_SIZE 1024
#define MAX_ITERATIONS 1000

void random_alloc_free_test() 
{
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) 
    {
        int index = rand() % NUM_ALLOCS;
        if (pointers[index] == NULL) 
        {
            // Allocate memory
            size_t size = (size_t)(rand() % MAX_SIZE) + 1;
            pointers[index] = HmmAlloc(size);
            if (pointers[index] != NULL) 
            {
                printf("Allocated memory of size %zu at address %p\n", size, pointers[index]);
            } 
            else 
            {
                fprintf(stderr, "Allocation failed for size %zu\n", size);
            }
        } 
        else 
        {
            // Free memory
            printf("Freeing memory at address %p\n", pointers[index]);
            HmmFree(pointers[index]);
            pointers[index] = NULL;
        }
    }
    
    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) 
    {
        if (pointers[i] != NULL) 
        {
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            HmmFree(pointers[i]);
            pointers[i] = NULL;
        }
    }
}

int main() 
{
    printf("Starting random allocation and deallocation test...\n");
    random_alloc_free_test();
    printf("Test complete.\n");
    return 0;
}
#endif

#if 1
int main()
{
	printf("ptr1 malloc(100)\n");
    char* prt1 = (char*)malloc(100);
    printf("ptr1 = %p\n",prt1);
how_are_heap();
    printf("ptr2 malloc(24)\n");
    char* prt2 = (char*)malloc(24);
    printf("ptr2 = %p\n",prt2);
how_are_heap();
    printf("ptr3 realloc(NULL,200)\n");
    char* prt3 = (char*)realloc(NULL,200);
    printf("ptr3 = %p\n",prt3);
how_are_heap();
    HmmFree(prt1);
how_are_heap();
    printf("ptr4 malloc(24)\n");
    char* prt4 = (char*)malloc(24);
    printf("ptr4 = %p\n",prt2);

    how_are_heap();

    return 0;
}
#endif