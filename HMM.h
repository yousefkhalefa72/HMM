char  how_are_heap();
void *HmmAlloc(size_t size);
void *HmmCalloc(size_t nmemb, size_t size);
void  HmmFree(void *ptr);
void *HmmRealloc(void *ptr, size_t size);
void DestroyHeap();