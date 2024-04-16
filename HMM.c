// to use printf 
#include <stdio.h>
// to use sbrk
#include <stdlib.h>
#include <unistd.h>

// size of free metadata 8+8+8+8
#define fmdSize    32
// size of allocated metadata 
#define amdSize    8
// size of new allocated area
#define namSize    500
// size of max free mem in last node
#define mfmSize    2000

//====================================== Data Types ======================================================
typedef struct free_Block 
{
    size_t size;
    struct free_Block *prev;
    struct free_Block *next;
    void* free_mem; 
}free_Block_t;

// Define memory blocks
typedef struct allocated_Block 
{
    size_t size;
}allocated_Block_t;

//======================================== Functions ======================================================
unsigned char how_are_heap(free_Block_t* heap_start);
void *HmmAlloc(size_t size);
void *HmmCalloc(size_t nmemb, size_t size);
unsigned char HmmFree(void **ptr);
void *HmmRealloc(void *ptr, size_t size);

//======================================== Program ========================================================
free_Block_t *heap_start = NULL;

int main()
{
    // init the heap with size 10k
    heap_start = (free_Block_t *)sbrk(namSize);
    heap_start->size=(namSize)-fmdSize;
    heap_start->prev=NULL;
    heap_start->next=NULL;
    heap_start->free_mem=(free_Block_t*)((char*)heap_start+24);
    how_are_heap(heap_start);

    printf("                    HmmAlloc(200)");
    char* ptr1=(char*)HmmAlloc(200);
    how_are_heap(heap_start);

    printf("                    HmmAlloc(150)");
    char* ptr2=(char*)HmmAlloc(150);
    how_are_heap(heap_start);

    printf("                    HmmAlloc(400)");
    char* ptr3=(char*)HmmAlloc(400);
    how_are_heap(heap_start);

    printf("                    HmmAlloc(600)");
    char* ptr4=(char*)HmmAlloc(600);
    how_are_heap(heap_start);
    
    printf("                    HmmFree(ptr1)");
    HmmFree(ptr1);
    how_are_heap(heap_start);

    printf("                    HmmFree(ptr3)");
    HmmFree(ptr3);
    how_are_heap(heap_start);

    printf("                    HmmAlloc(10)");
    ptr1=(char*)HmmAlloc(10);
    how_are_heap(heap_start);

    printf("                    HmmAlloc(60)");
    ptr3=(char*)HmmAlloc(60);
    how_are_heap(heap_start);

    printf("                    HmmFree(ptr4)");
    HmmFree(ptr4);
    how_are_heap(heap_start);

    printf("                    HmmFree(ptr2)");
    HmmFree(ptr2);
    how_are_heap(heap_start);

    printf("                    HmmFree(ptr3)");
    HmmFree(ptr3);
    how_are_heap(heap_start);

    printf("                    HmmFree(ptr4)");
    HmmFree(ptr1);
    how_are_heap(heap_start);

    return 0;
}

//============================================ How are heap ===========================================
unsigned char how_are_heap(free_Block_t* heap_start)
{
    if(heap_start == NULL)
    {
        printf("Usage : how_are_heap => heap_start\n");
        return -1;
    }
    free_Block_t* block = heap_start;

    int i=1;
    printf("\n");

    while(block->next !=NULL)
    {
        if(block->size != 0)
        {
            //green output
            printf("\e[0;32m%d-fmd  add=0x%6x  size=%6d    prev_node=0x%08x    next_node=0x%08x   PtrToFreeMem=0x%x\e[0m\n",
                i,block,block->size,block->prev,block->next,block->free_mem);
        }
        else
        {
            // red output
            printf("\e[0;31m%d-fmd  add=0x%6x  size=%6d    prev_node=0x%08x    next_node=0x%08x\e[0m\n",
                i,block,block->size,block->prev,block->next);
            //blue output
            printf("\e[1;34m%d-amd  add=0x%6x  size=%6d    PtrToAMem=0x%x\e[0m\n",
                i,(char*)block+24,(size_t)(block->free_mem),(char*)block+fmdSize);
        }
        block=block->next;
        i++;
    }
    printf("\e[0;32m%d-fmd  add=0x%6x  size=%6d    prev_node=0x%08x    next_node=0x%08x   PtrToFreeMem=0x%x\e[0m\n\n",
                i,block,block->size,block->prev,block->next,block->free_mem);

    return 0;
}

//========================================= HmmAlloc ==================================================
void *HmmAlloc(size_t size) 
{
    free_Block_t *first_free = heap_start;

    if(first_free == NULL)
    {
        printf("Usage : HmmAlloc => heap_start\n");
        return NULL;
    }

    if(size == 0)
    {
        return NULL;
    }

    // search free ptr from linkedlist
    while (first_free->size < size+fmdSize) 
    {
        if(first_free->next == NULL)
        {
            free_Block_t* new_free = (free_Block_t*)sbrk(namSize+size);
            if(new_free == -1)
            {
                printf("faild: to allocate memory\n");
                return NULL;
            }
            new_free->size=(namSize+size)-fmdSize+ first_free->size;
            new_free->prev=first_free;
            new_free->next=NULL;
            new_free->free_mem=(free_Block_t*)((char*)new_free+24);

            first_free->next = new_free;
            first_free=first_free->next;
            break;
            // printf("\nFailed: couldn't allocate");
            // return NULL;
        }
        else if(first_free->size == size)
        {
            // create allcated node s=s , return ptr 
            allocated_Block_t* New_Ablock = first_free->free_mem;
            New_Ablock->size=size;
            void* PtrPointData=((char*)New_Ablock+8);

            first_free->size = 0;
            return PtrPointData;
        }
        else
        {
            first_free = first_free->next;
        }

    }

    // create allcated node s=s , return ptr 
    allocated_Block_t* New_Ablock = first_free->free_mem;
    New_Ablock->size=size;
    void* PtrPointData=((char*)New_Ablock+8);

    free_Block_t* after_allocated_block= (free_Block_t*)((char*)first_free+fmdSize+size);
    after_allocated_block->size = (first_free->size)-size-fmdSize;
    after_allocated_block->next = first_free->next;
    after_allocated_block->prev = first_free;
    after_allocated_block->free_mem = (free_Block_t*)((char*)after_allocated_block+fmdSize-8);

    first_free->size = 0;
    first_free->next = after_allocated_block;
    if(after_allocated_block->next != NULL)
    {
        (after_allocated_block->next)->prev = after_allocated_block;
    }

    return PtrPointData;
}
//======================================= HmmCalloc ====================================================
void *HmmCalloc(size_t nmemb, size_t size)
{
    if((nmemb == 0) || (size ==0))
    {
        return NULL;
    }
    return HmmAlloc(nmemb*size);
}

//======================================= HmmFree ======================================================
unsigned char HmmFree(void **ptr) 
{
    if (ptr == NULL) 
    {
        return -1; 
    }

    // metadata = ptr -32(8+24)
    allocated_Block_t *block = (allocated_Block_t*)((char*)ptr - amdSize);

    free_Block_t* down_free = (free_Block_t*)((char*)block-24);
    free_Block_t* up_free = down_free->next;

    if(up_free->size == 0)
    {
        down_free->size = block->size;
    }
    else //merga
    {
        while(up_free->size != 0)
        {
            // if the up free is the last free of heap
            if(up_free->next == NULL)
            {
                down_free->next=NULL;
                down_free->size = block->size + fmdSize + up_free->size;
                break;
            }
            else
            {
                down_free->next=up_free->next;
                (up_free->next)->prev=down_free;
                down_free->size = block->size + fmdSize + up_free->size;
                up_free = down_free->next;
            }
            // up_free->next=NULL;up_free->prev=NULL;up_free->size=0;up_free->free_mem=NULL;
        }
    }
    down_free->free_mem = block;
    ptr = NULL;

    free_Block_t* down_down_free = down_free->prev;
    if(down_down_free != NULL)
    {
        if(down_down_free->size != 0)
        {
            down_down_free->next = down_free->next;
            if(down_free->next != NULL)
            {
                (down_free->next)->prev = down_down_free;
            }
            down_down_free->size += down_free->size + fmdSize;
            down_free = NULL;
        }
    }

    free_Block_t* last_free_node = heap_start;
    while(last_free_node->next != NULL)
    {
        last_free_node = last_free_node->next;
    }
    while(last_free_node->size > mfmSize)
    {
        sbrk(-2000);
        last_free_node->size -=2000;
    }

    return 0;
}

//======================================= Hmmrealloc ==========================================
void *HmmRealloc(void *ptr, size_t size)
{
    if(size == 0)
    {
        HmmFree(ptr);
        return NULL;
    }
    
    allocated_Block_t* aBlock = (allocated_Block_t*)((char*)ptr -amdSize);
    free_Block_t* fBlock = (free_Block_t*)((char*)ptr -fmdSize);

    if(size == aBlock->size)
    {
        return ptr;
    }
    else if(size < (aBlock->size)+fmdSize)
    {
        aBlock->size = size;
        free_Block_t* after_allocated_block= (free_Block_t*)((char*)fBlock+fmdSize+size);
        after_allocated_block->size = size-aBlock->size-fmdSize;
        after_allocated_block->next = fBlock->next;
        after_allocated_block->prev = fBlock;
        after_allocated_block->free_mem = (free_Block_t*)((char*)after_allocated_block+fmdSize-8);
        fBlock->next = after_allocated_block;
    }
    else if(size > (aBlock->size)+fmdSize)
    {
        void* new_ptr = (void*)HmmAlloc(size);
        if(new_ptr == NULL)
        {
            printf("Failed: to realloc new mem\n");
            return NULL;
        }

        size_t copy_size = 0;
        while(copy_size < aBlock->size)
        {
            *((char*)new_ptr+copy_size) = *((char*)ptr+copy_size);
        }
        
        HmmFree(&ptr);
        return new_ptr;
    }
}