# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>

struct MemoryBlock
{
  int allocatedID;
  bool isAllocated;
  int size;
  int startOffset;
};

bool validateMemoryPoolSize(int size);

void displayMenu();

void displaySelectedStrategy(int choice);

void allocateMemory(int strategy, int poolSize, struct MemoryBlock **blocks, int *count, int *capacity, int *nextID);

int findSuitableBlock(int strategy, int size, struct MemoryBlock *blocks, int count);

void releaseMemory(struct MemoryBlock *blocks, int *count);

int isBlockExists(int id, struct MemoryBlock *blocks, int count);

void mergeFreeBlocks(struct MemoryBlock *blocks, int *count);

void displayMemoryMap(struct MemoryBlock *blocks, int count, int poolSize);

void displayMemoryStatistics(struct MemoryBlock *blocks, int count, int poolSize);

void changeAllocationStrategy(int *strategyChoice);

void resizeAllocation(struct MemoryBlock **blocks, int *count, int *capacity, char *pool, int poolSize);

int main()
{
  int memoryPoolSize;
  int menuChoice;
  int allocationStrategyChoice;

  printf("Enter the size of the memory pool(1 - 1000000): ");
  scanf("%d", &memoryPoolSize);

  if (!validateMemoryPoolSize(memoryPoolSize))
  {
    return 0;
  }

  char *memoryPool = (char *)malloc(memoryPoolSize * sizeof(char));

  if (memoryPool == NULL)
  {
    printf("Memory allocation failed. Exiting the program.\n");
    return 0;
  }

  printf("Select Allocation Strategy: (1. First Fit 2. Best Fit 3. Worst Fit)\n");
  scanf("%d", &allocationStrategyChoice);

  if (allocationStrategyChoice < 1 || allocationStrategyChoice > 3)
  {
    printf("Invalid choice. Please select a valid allocation strategy.\n");
    free(memoryPool);
    return 0;
  }

  displaySelectedStrategy(allocationStrategyChoice);

  struct MemoryBlock *memoryBlocks = (struct MemoryBlock *)malloc(2*sizeof(struct MemoryBlock));

  if (memoryBlocks == NULL)
  {
    printf("Memory allocation failed. Exiting the program.\n");
    free(memoryPool);
    return 0;
  }

  (memoryBlocks + 0)->allocatedID = 0;
  (memoryBlocks + 0)->isAllocated = false;
  (memoryBlocks + 0)->size = memoryPoolSize;
  (memoryBlocks + 0)->startOffset = 0;

  int blockCapacity = 2;
  int blockCount = 1;
  int nextAllocatedID = 1;

  do
  {
    printf("\n\n");

    displayMenu();

    printf("Enter your choice: ");
    scanf("%d", &menuChoice);

    switch (menuChoice)
    {
      case 1:
        // Allocate Memory
        allocateMemory(allocationStrategyChoice, memoryPoolSize, &memoryBlocks, &blockCount, &blockCapacity, &nextAllocatedID);
        break;

      case 2:
        // Release Memory
        releaseMemory(memoryBlocks, &blockCount);
        break;

      case 3:
        // Resize Allocation
         resizeAllocation(&memoryBlocks, &blockCount, &blockCapacity, memoryPool, memoryPoolSize);
        break;

      case 4:
        // Display Memory Map
        displayMemoryMap(memoryBlocks, blockCount, memoryPoolSize);
        break;

      case 5:
        // Display Memory statistics
        displayMemoryStatistics(memoryBlocks, blockCount, memoryPoolSize);
        break;

      case 6:
        // Change Memory Allocation Strategy
        changeAllocationStrategy(&allocationStrategyChoice);
        break;

      case 7:
        printf("Exiting the program.\n");
        break;

      default:
        printf("Invalid choice. Please try again.\n");
    }
  }
  while (menuChoice != 7);

  free(memoryPool);
  free(memoryBlocks);

}

bool validateMemoryPoolSize(int size)
{
  if (size < 1 || size > 1000000)
  {
    printf("Invalid memory pool size. Please enter a value between 1 and 1000000.\n");
    return false;
  }
  return true;
}

void displayMenu()
{
  printf("1. Allocate Memory\n");
  printf("2. Release Memory\n");
  printf("3. Resize Allocation\n");
  printf("4. Display Memory Map\n");
  printf("5. Display Memory statistics\n");
  printf("6. Change Memory Allocation Strategy\n");
  printf("7. Exit\n");
}

void displaySelectedStrategy(int choice)
{
  switch (choice)
  {
    case 1:
      printf("Selected Allocation Strategy: First Fit\n");
      break;
    case 2:
      printf("Selected Allocation Strategy: Best Fit\n");
      break;
    case 3:
      printf("Selected Allocation Strategy: Worst Fit\n");
      break;
  }
}

void allocateMemory(int strategy, int poolSize, struct MemoryBlock **blocks, int *count, int *capacity, int *nextID)
{
  int size;

  printf("Enter the size of memory to allocate: ");
  scanf("%d", &size);

  if (size <= 0 || size > poolSize)
  {
    printf("Invalid size.\n");
    return;
  }

  int block = findSuitableBlock(strategy, size, *blocks, *count);

  if (block == -1)
  {
    printf("No suitable block found for allocation.\n");
    return;
  }

  int originalSize = (*blocks)[block].size;
  int originalStartOffset = (*blocks)[block].startOffset;

  if (originalSize > size)
  {
    if (*count >= *capacity)
    {
      int newCapacity = *capacity*2;
      struct MemoryBlock *temp = (struct MemoryBlock *)realloc(*blocks, (newCapacity) * sizeof(struct MemoryBlock));

      if (temp == NULL)
      {
        printf("Memory reallocation failed.Previous records retained.\n");
        return;
      }
      *capacity = newCapacity;
      *blocks = temp;
    }

      for (int i = *count; i > block + 1; i--)
      {
        (*blocks)[i] = (*blocks)[i - 1];
      }

      (*blocks)[block + 1].size = originalSize - size;
      (*blocks)[block + 1].startOffset = originalStartOffset + size;
      (*blocks)[block + 1].isAllocated = false;
      (*blocks)[block + 1].allocatedID = 0;

      (*count)++;

  }
  (*blocks)[block].isAllocated = true;
  (*blocks)[block].allocatedID = *nextID;
  (*nextID)++;
  (*blocks)[block].size = size;

  printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", (*blocks)[block].allocatedID, (*blocks)[block].startOffset, (*blocks)[block].size);
}

int findSuitableBlock(int strategy, int size, struct MemoryBlock *blocks, int count)
{
  switch(strategy)
  {
    case 1: // first fit
    {
      for (int i = 0; i < count; i++)
      {
        if (!blocks[i].isAllocated && blocks[i].size >= size)
        {
          return i;
        }
      }
      return -1;
      break;
    }
    
    case 2: // best fit
    {
      int bestIndex = -1;
      
      int bestSize = size;

      for (int i = 0; i < count; i++)
      {
        if (!blocks[i].isAllocated && blocks[i].size >= size)
        {
         if (bestIndex == -1)
         {
            bestIndex = i;
            bestSize = blocks[i].size;
          }

          else if (blocks[i].size < bestSize)
          {
            bestIndex = i;
            bestSize = blocks[i].size;
         }
        }
      }
      return bestIndex;
      break;
    }
    
    case 3: // worst fit
    {
      int worstIndex = -1;
      int worstSize = 0;

      for (int i = 0; i < count; i++)
      {
        if (!blocks[i].isAllocated && blocks[i].size >= size)
        {
          if (worstIndex == -1)
          {
            worstIndex = i;
            worstSize = blocks[i].size;
          }
          else if (blocks[i].size > worstSize)
          {
            worstIndex = i;
            worstSize = blocks[i].size;
          }
        }
      }
      return worstIndex;
      break;
    }
  }
}


void releaseMemory(struct MemoryBlock *blocks, int *count)
{
  int id;

  printf("Enter the ID of the memory block to release: ");
  scanf("%d", &id);

  int block = isBlockExists(id, blocks, *count);
  if (block == -1)
  {
    printf("No allocated block found with ID %d.\n", id);
    return;
  }

  blocks[block].isAllocated = false;
  blocks[block].allocatedID = 0;

  mergeFreeBlocks(blocks, count);
  printf("Memory block with ID %d released successfully.\n", id);
}

int isBlockExists(int id, struct MemoryBlock *blocks, int count)
{
  for (int i = 0; i < count; i++)
  {
    if (blocks[i].allocatedID == id && blocks[i].isAllocated)
    {
      return i;
    }
  }
 
  return -1;
}

void mergeFreeBlocks(struct MemoryBlock *blocks, int *count)
{
  int i = 0;

  while (i < *count - 1)
  {
    if (!blocks[i].isAllocated && !blocks[i + 1].isAllocated)
    {
      blocks[i].size += blocks[i + 1].size;

      for (int j = i + 1; j < *count - 1; j++)
      {
        blocks[j] = blocks[j + 1];
      }

      (*count)--;

    }
    else
    {
      i++;
    }
  }
}

void displayMemoryMap(struct MemoryBlock *blocks, int count, int poolSize)
{
  printf("Memory Map(%d bytes)\n", poolSize);

  for (int i = 0; i < count; i++)
  {
    if (blocks[i].isAllocated)
    {
      printf("Allocated Id %d: %d bytes, Start: %d\n", blocks[i].allocatedID, blocks[i].size, blocks[i].startOffset);
    }
    else{
      printf("Free: %d bytes, Start: %d\n", blocks[i].size, blocks[i].startOffset);
    }
  }
}


void displayMemoryStatistics(struct MemoryBlock *blocks, int count, int poolSize)
{
  printf("Total Memory: %d bytes\n", poolSize);
  int allocatedMemory = 0;
  int freeMemory = 0;
  int freeBlocks = 0;
  int largestFreeBlock = 0;

  for (int i = 0; i < count; i++)
  {
    if (blocks[i].isAllocated)
    {
      allocatedMemory += blocks[i].size;
    }
    else
    {
      freeMemory += blocks[i].size;
      freeBlocks++;

      if (largestFreeBlock == 0)
      {
        largestFreeBlock = blocks[i].size;
      }
      else if (blocks[i].size > largestFreeBlock)
      {
        largestFreeBlock = blocks[i].size;
      }
    }
  }

  printf("Allocated Memory: %d bytes\n", allocatedMemory);
  printf("Free Memory: %d bytes\n", freeMemory);
  printf("Number of Free Blocks: %d\n", freeBlocks);
  printf("Largest Free Block: %d bytes\n", largestFreeBlock);
}

void changeAllocationStrategy(int *strategyChoice)
{
  int newChoice;

  printf("Select Allocation Strategy: (1. First Fit 2. Best Fit 3. Worst Fit)\n");
  scanf("%d", &newChoice);

  *strategyChoice = newChoice;
  displaySelectedStrategy(*strategyChoice);
}

void resizeAllocation(struct MemoryBlock **blocks, int *count, int *capacity, char *pool, int poolSize)
{
  int id, newSize;

  printf("Enter the ID of the memory block to resize: ");

  scanf("%d", &id);

  int block = isBlockExists(id, *blocks, *count);

  if (block == -1)
  {
    printf("No allocated block found with ID %d.\n", id);
    return;
  }

  printf("Enter the new size: ");

  if (scanf("%d", &newSize) != 1)
  {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
    printf("Invalid input.\n");
    return;
  }

  if (newSize <= 0 || newSize > poolSize)
  {
    printf("Invalid size. Enter between 1 and %d bytes.\n", poolSize);
    return;
  }

  int oldSize = (*blocks)[block].size;
  int oldStart = (*blocks)[block].startOffset;

  if (newSize == oldSize)
  {
    printf("Allocation already has this size.\n");
    return;
  }

  if (newSize < oldSize)
  {
    int releasedSize = oldSize - newSize;

    if (block + 1 < *count &&
        !(*blocks)[block + 1].isAllocated)
    {
      /* Extend the following free region backward. */
      (*blocks)[block + 1].startOffset = oldStart + newSize;
      (*blocks)[block + 1].size += releasedSize;
    }
    else
    {
      if (*count >= *capacity)
      {
        int newCapacity =
            (*capacity > poolSize / 2) ? poolSize : *capacity * 2;

        struct MemoryBlock *temp =
            realloc(*blocks, (size_t)newCapacity * sizeof **blocks);

        if (temp == NULL)
        {
          printf("Resize failed. Original allocation unchanged.\n");
          return;
        }

        *blocks = temp;
        *capacity = newCapacity;
      }

      for (int i = *count; i > block + 1; i--)
      {
        (*blocks)[i] = (*blocks)[i - 1];
      }

      (*blocks)[block + 1].allocatedID = 0;
      (*blocks)[block + 1].isAllocated = false;
      (*blocks)[block + 1].size = releasedSize;
      (*blocks)[block + 1].startOffset = oldStart + newSize;

      (*count)++;
    }

    (*blocks)[block].size = newSize;
    mergeFreeBlocks(*blocks, count);

    printf("Allocation %d shrunk to %d bytes.\n", id, newSize);
    return;
  }

  int extraNeeded = newSize - oldSize;

  if (block + 1 < *count &&
      !(*blocks)[block + 1].isAllocated &&
      (*blocks)[block + 1].size >= extraNeeded)
  {
    if ((*blocks)[block + 1].size == extraNeeded)
    {
      for (int i = block + 1; i < *count - 1; i++)
      {
        (*blocks)[i] = (*blocks)[i + 1];
      }

      (*count)--;
    }
    else
    {
      (*blocks)[block + 1].startOffset += extraNeeded;
      (*blocks)[block + 1].size -= extraNeeded;
    }

    (*blocks)[block].size = newSize;

    printf("Allocation %d expanded in place to %d bytes.\n",
           id, newSize);
    return;
  }

  int destination = findSuitableBlock(1, newSize, *blocks, *count);

  if (destination == -1)
  {
    printf("Resize failed: no suitable free region.\n");
    printf("Original allocation and data remain unchanged.\n");
    return;
  }

  int destinationSize = (*blocks)[destination].size;
  int destinationStart = (*blocks)[destination].startOffset;

  if (destinationSize > newSize && *count >= *capacity)
  {
    int newCapacity =
        (*capacity > poolSize / 2) ? poolSize : *capacity * 2;

    struct MemoryBlock *temp =
        realloc(*blocks, (size_t)newCapacity * sizeof **blocks);

    if (temp == NULL)
    {
      printf("Resize failed. Original allocation unchanged.\n");
      return;
    }

    *blocks = temp;
    *capacity = newCapacity;
  }

  for (int i = 0; i < oldSize; i++)
  {
  *(pool + destinationStart + i) = *(pool + oldStart + i);
  }

  if (destinationSize > newSize)
  {
    for (int i = *count; i > destination + 1; i--)
    {
      (*blocks)[i] = (*blocks)[i - 1];
    }

    (*blocks)[destination + 1].allocatedID = 0;
    (*blocks)[destination + 1].isAllocated = false;
    (*blocks)[destination + 1].size = destinationSize - newSize;
    (*blocks)[destination + 1].startOffset =
        destinationStart + newSize;

    (*count)++;

   
    if (destination < block)
    {
      block++;
    }
  }

  (*blocks)[block].isAllocated = false;
  (*blocks)[block].allocatedID = 0;

  (*blocks)[destination].isAllocated = true;
  (*blocks)[destination].allocatedID = id;
  (*blocks)[destination].size = newSize;

  mergeFreeBlocks(*blocks, count);

  printf("Allocation %d moved from offset %d to %d, size %d bytes.\n",
         id, oldStart, destinationStart, newSize);
}