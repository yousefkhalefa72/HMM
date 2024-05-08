// to use printf 
#include <stdio.h>

// to use sbrk
#include <unistd.h>

// #define BEST_FIT

// size of free metadata 8+8+8+8
#define fmdSize    32
// size of allocated metadata 
#define amdSize    8
// size of new allocated area
#define namSize    2000
// size of max free mem in last node
#define mfmSize    4096
// max size of allocation
#define msoAlloc   1000000


typedef struct free_Block 
{
    size_t size;
    struct free_Block *prev;
    struct free_Block *next;
    void *free_mem; 
}free_Block_t;

typedef struct allocated_Block 
{
    size_t size;
}allocated_Block_t;



static free_Block_t* heap_start = NULL;

char how_are_heap()
{
    if(heap_start == NULL)
    {
        printf("NO Init Heap.\n");
        return 0;
    }
    free_Block_t* block = heap_start;

    int i=1;
    printf("\n");

    while(block->next !=NULL)
    {
        if(block->size != 0)
        {
            //green output
            printf("\e[0;32m%d-fmd  add=0x%p  size=%6ld    prev_node=0x%14p    next_node=0x%14p   PtrToFreeMem=0x%p\e[0m\n",
                i,block,block->size,block->prev,block->next,block->free_mem);
        }
        else
        {
            // red output
            printf("\e[0;31m%d-fmd  add=0x%p  size=%6ld    prev_node=0x%14p    next_node=0x%p\e[0m\n",
                i,block,block->size,block->prev,block->next);
            //blue output
            printf("\e[1;34m%d-amd  add=0x%p  size=%6ld    PtrToAMem=0x%p\e[0m\n",
                i,(char*)block+24,(size_t)(block->free_mem),(char*)block+fmdSize);
        }
        block=block->next;
        i++;
    }
    printf("\e[0;32m%d-fmd  add=0x%p  size=%6ld    prev_node=0x%14p    next_node=0x%14p   PtrToFreeMem=0x%p\e[0m\n\n",
                i,block,block->size,block->prev,block->next,block->free_mem);

    return i;
}

void *HmmAlloc(size_t size) 
{
    size = ((size+7)/8)*8; // Aligning size to 8 bytes
    if(size == 0)
    {
        return HmmAlloc(1);
    }
    
    if(size > msoAlloc)
    {
        // printf("usage: size = %ld => Not valid value.\n",size);
        return NULL;
    }

    free_Block_t *first_free = heap_start;
    if(first_free == NULL)
    {
        // printf("init the heap \n");
        heap_start = (free_Block_t *)sbrk(namSize);
        heap_start->size=(namSize)-fmdSize;
        heap_start->prev=NULL;
        heap_start->next=NULL;
        heap_start->free_mem=(free_Block_t*)((char*)heap_start+24);
        first_free = heap_start;
    }

#ifdef BEST_FIT
    // best fit
    free_Block_t *best_fit = NULL;
    size_t best_fit_size = msoAlloc;

    // find the best fitting block
    free_Block_t *current = heap_start;
    for (; current->next != NULL; current = current->next) 
    {
        if ((current->size >= size + fmdSize) && (current->size < best_fit_size)) 
        {
            best_fit = current;
            best_fit_size = current->size;
            // Perfect fit
            if (current->size == size) 
            {  
                // create allcated node s=s , return ptr 
                allocated_Block_t* New_Ablock = current->free_mem;
                New_Ablock->size=size;
                void* PtrPointData=((char*)New_Ablock+8);

                current->size = 0;
                return PtrPointData;
            }
        }
    }
    //check lastt node
    if ((current->size >= size + fmdSize) && (current->size < best_fit_size)) 
    {
        best_fit = current;
        best_fit_size = current->size;
        // Perfect fit
        if (current->size == size) 
        {  
            // create allcated node s=s , return ptr 
            allocated_Block_t* New_Ablock = current->free_mem;
            New_Ablock->size=size;
            void* PtrPointData=((char*)New_Ablock+8);

            current->size = 0;
            return PtrPointData;
        }
    }

    // No suitable block found, expand the heap
    if (best_fit == NULL) 
    {  
        void* ptr = sbrk(namSize+size);
            if(ptr == (void*)-1)
            {
                // printf("faild: to allocate memory\n");
                return NULL;
            }
        current->size +=(namSize+size);
        first_free = current;
    } 
    else 
    {  
        first_free = best_fit;
    }
#endif

#ifndef BEST_FIT
    // first fit
    // search free ptr from linkedlist
    while (first_free->size < size+fmdSize) 
    {
        if(first_free->next == NULL)
        {
            void* ptr = sbrk(namSize+size);
            if(ptr == (void*)-1)
            {
                // printf("faild: to allocate memory\n");
                return NULL;
            }
            first_free->size +=(namSize+size);
            break;
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
        first_free = first_free->next;
    }
    #endif
    // create allcated node o_size=n_size , return ptr 
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

void *HmmCalloc(size_t nmemb, size_t size)
{
    if((nmemb == 0) || (size ==0))
    {
        return NULL;
    }

    size_t L_size = nmemb*size;
    void* ptr = HmmAlloc(L_size);
    if(ptr == NULL)
    {
        return NULL;
    }

    size_t counter = 0;
    while(counter < L_size)
    {
        *((char*)ptr+counter) = 0;
        counter++;
    }
    return ptr;
}

void HmmFree(void *ptr) 
{
    if (ptr == NULL) 
    {
        return; //error passing NULL ptr
    }

    // metadata = ptr -32(8+24)
    allocated_Block_t *block = (allocated_Block_t*)((char*)ptr - amdSize);

    free_Block_t* down_free = (free_Block_t*)((char*)block - (fmdSize - amdSize));
    free_Block_t* up_free = down_free->next;

    if(down_free->size != 0)
    {
        return; // The block is already freed or invalid operation.
    }

    down_free->size = block->size;
    down_free->free_mem = block;

    // Merge with the next block if it is free.
    if ((up_free) && (up_free->size != 0)) 
    {
        down_free->size += up_free->size + fmdSize;
        down_free->next = up_free->next;
        if (up_free->next) 
        {
            up_free->next->prev = down_free;
        }
    }

    free_Block_t* down_down_free = down_free->prev;

    // Try to merge with the previous block if it's free.
    if (down_down_free && down_down_free->size != 0) 
    {
        down_down_free->size += down_free->size + fmdSize;
        down_down_free->next = down_free->next;
        if(down_free->next != NULL) 
        {
            down_free->next->prev = down_down_free;
        }
    }
    
    // if the last block is free and large enough. 
    //       try 1
    // if(up_free)
    // {
    //     if((up_free->next == NULL)&&(up_free->size > mfmSize))
    //     {
    //         sbrk(-(up_free->size) + mfmSize);
    //         up_free->size = mfmSize;
    //     }
    // }
    // else if(down_free)
    // {
    //     if(down_free->size > mfmSize)
    //     {
    //         sbrk(-(down_free->size) + mfmSize);
    //         down_free->size = mfmSize;
    //     }
    // }
    // else if(down_down_free)
    // {
    //     if(down_down_free->size > mfmSize)
    //     {
    //         sbrk(-(down_down_free->size) + mfmSize);
    //         down_down_free->size = mfmSize;
    //     }
    // }
    //         try 2
    // if(down_down_free->next == NULL) 
    // {
    //     if(down_down_free->size > mfmSize)
    //     {
    //         sbrk(-(down_down_free->size) + mfmSize);
    //         down_down_free->size = mfmSize;
    //     }
    // }
    // else if(down_free->next == NULL) 
    // {
    //     if(down_free->size > mfmSize)
    //     {
    //         sbrk(-(down_free->size) + mfmSize);
    //         down_free->size = mfmSize;
    //     }
    // }
    //    passed
    if(down_down_free != NULL)
    {
        while(down_down_free->next != NULL)
        {
            down_down_free = down_down_free->next;
        }
        
        if(down_down_free->size > mfmSize)
        {
            sbrk(-(down_down_free->size) + mfmSize);
            down_down_free->size = mfmSize;
        }
    }
}

void *HmmRealloc(void *ptr, size_t size)
{
    if(ptr == NULL)
    {
        return HmmAlloc(size);
    }

    if(size == 0)
    {
        HmmFree(ptr);
        return NULL;
    }
    
    size = ((size+7)/8)*8; // alignedSize
    
    allocated_Block_t* old_block = (allocated_Block_t*)((char*)ptr -amdSize);
    free_Block_t* fBlock = (free_Block_t*)((char*)ptr -fmdSize);

    if(size == old_block->size)
    {
        return ptr;
    }
    else if(size+fmdSize < (old_block->size))
    {
        size_t old_size = old_block->size;
        old_block->size = size;

        free_Block_t* after_allocated_block= (free_Block_t*)((char*)fBlock+fmdSize+size);
        after_allocated_block->size = old_size - old_block->size - fmdSize;
        after_allocated_block->next = fBlock->next;
        after_allocated_block->prev = fBlock;
        after_allocated_block->free_mem = (free_Block_t*)((char*)after_allocated_block+fmdSize-8);
        fBlock->next = after_allocated_block;
        (after_allocated_block->next)->prev =after_allocated_block;
    }
    else
    {
        void* new_ptr = HmmAlloc(size);
        if(new_ptr == NULL)
        {
            // printf("Failed: to realloc new mem\n");
            return NULL;
        }

        size_t copy_size = 0;
        while(copy_size < (old_block->size))
        {
            *((char*)new_ptr+copy_size) = *((char*)ptr+copy_size);
            copy_size++;
        }
        
        HmmFree(ptr);
        return new_ptr;
    }
}

void DestroyHeap()
{
    if(heap_start->next != NULL)
    {
        printf("\e[0;31mbefore destroy heap ,you must free all heap\e[0m" );
        return;
    }
    sbrk(-(heap_start->size + 32));
    heap_start = NULL;
}

#if 1
void *malloc(size_t size)
{
    return HmmAlloc(size);
}
void free(void *ptr)
{
    HmmFree(ptr);
}
void *calloc(size_t nmemb, size_t size)
{
    return HmmCalloc(nmemb,size);
}
void *realloc(void *ptr, size_t size)
{
    return HmmRealloc(ptr,size);
}
#endif
