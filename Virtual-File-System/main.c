#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 100
#define NAME_SIZE 51
#define INPUT_SIZE 8192

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *prev;
    struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode
{
    char name[NAME_SIZE];
    int isDirectory;

    int size;
    int blockCount;
    int blockPointers[NUM_BLOCKS];

    struct FileNode *parent;
    struct FileNode *children;
    struct FileNode *next;
} FileNode;

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

FreeBlock freeBlocks[NUM_BLOCKS];

FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;

int freeCount = 0;

FileNode *root = NULL;
FileNode *cwd = NULL;

/* Add a block at the end of the free list */

void returnBlock(int index)
{
    FreeBlock *block = &freeBlocks[index];

    block->index = index;
    block->prev = freeTail;
    block->next = NULL;

    if (freeTail == NULL)
    {
        freeHead = block;
    }
    else
    {
        freeTail->next = block;
    }

    freeTail = block;
    freeCount++;
}

/* Remove a block from the beginning of the free list */

int allocateBlock(void)
{
    if (freeHead == NULL)
    {
        return -1;
    }

    FreeBlock *block = freeHead;
    int index = block->index;

    freeHead = freeHead->next;

    if (freeHead == NULL)
    {
        freeTail = NULL;
    }
    else
    {
        freeHead->prev = NULL;
    }

    block->prev = NULL;
    block->next = NULL;

    freeCount--;

    return index;
}

/* Release all blocks belonging to a file */

void releaseFileBlocks(FileNode *file)
{
    for (int i = 0; i < file->blockCount; i++)
    {
        int index = file->blockPointers[i];

        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            virtualDisk[index][j] = 0;
        }

        returnBlock(index);
    }

    file->blockCount = 0;
    file->size = 0;
}

/* Search for a child inside a directory */

FileNode *findChild(FileNode *directory, const char *name)
{
    FileNode *first = directory->children;

    if (first == NULL)
    {
        return NULL;
    }

    FileNode *current = first;

    do
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }

        current = current->next;
    }
    while (current != first);

    return NULL;
}

/* Add a child to the directory's circular list */

void addChild(FileNode *directory, FileNode *node)
{
    node->parent = directory;

    if (directory->children == NULL)
    {
        directory->children = node;
        node->next = node;
        return;
    }

    FileNode *first = directory->children;
    FileNode *last = first;

    while (last->next != first)
    {
        last = last->next;
    }

    last->next = node;
    node->next = first;
}

/* Disconnect a child from the directory's circular list */

void removeChild(FileNode *directory, FileNode *node)
{
    FileNode *first = directory->children;

    if (node->next == node)
    {
        directory->children = NULL;
    }
    else
    {
        FileNode *previous = first;

        while (previous->next != node)
        {
            previous = previous->next;
        }

        previous->next = node->next;

        if (directory->children == node)
        {
            directory->children = node->next;
        }
    }

    node->next = NULL;
}

/* Check whether a new name is valid */

int validName(const char *name)
{
    int length = (int)strlen(name);

    if (length == 0 || length >= NAME_SIZE)
    {
        return 0;
    }

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    {
        return 0;
    }

    for (int i = 0; name[i] != '\0'; i++)
    {
        if (name[i] == '/')
        {
            return 0;
        }
    }

    return 1;
}

/* Create a file or directory */

void createEntry(const char *name, int isDirectory)
{
    if (!validName(name))
    {
        printf("Invalid name. Use 1-50 characters without '/'.\n");
        return;
    }

    if (findChild(cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *node = calloc(1, sizeof(FileNode));

    if (node == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    strcpy(node->name, name);

    node->isDirectory = isDirectory;
    node->parent = NULL;
    node->children = NULL;
    node->next = NULL;

    addChild(cwd, node);

    if (isDirectory)
    {
        printf("Directory created.\n");
    }
    else
    {
        printf("File created.\n");
    }
}

/* Write content into a file, replacing previous content */

void writeFile(const char *name, const char *content)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    int size = (int)strlen(content);
    int requiredBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (requiredBlocks > freeCount + file->blockCount)
    {
        printf("Not enough disk space. Old content preserved.\n");
        return;
    }

    releaseFileBlocks(file);

    int position = 0;

    for (int i = 0; i < requiredBlocks; i++)
    {
        int index = allocateBlock();

        for (int j = 0; j < BLOCK_SIZE && position < size; j++)
        {
            virtualDisk[index][j] = content[position];
            position++;
        }

        file->blockPointers[i] = index;
        file->blockCount++;
    }

    file->size = size;

    printf("Written %d bytes using %d block(s).\n", file->size, file->blockCount);
}

/* Read file content */

void readFile(const char *name)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    int printed = 0;

    for (int i = 0; i < file->blockCount; i++)
    {
        int index = file->blockPointers[i];

        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            printf("%c", virtualDisk[index][j]);
            printed++;
        }
    }

    printf("\n");
}

/* Delete a file */

void deleteFile(const char *name)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    releaseFileBlocks(file);
    removeChild(cwd, file);
    free(file);

    printf("File deleted.\n");
}

/* Remove an empty directory */

void removeDirectory(const char *name)
{
    FileNode *directory = findChild(cwd, name);

    if (directory == NULL || !directory->isDirectory)
    {
        printf("Directory not found.\n");
        return;
    }

    if (directory->children != NULL)
    {
        printf("Directory is not empty.\n");
        return;
    }

    removeChild(cwd, directory);
    free(directory);

    printf("Directory removed.\n");
}

/* List entries in the current directory */

void listEntries(void)
{
    FileNode *first = cwd->children;

    if (first == NULL)
    {
        printf("(empty)\n");
        return;
    }

    FileNode *current = first;

    do
    {
        if (current->isDirectory)
        {
            printf("%s/\n", current->name);
        }
        else
        {
            printf("%s\n", current->name);
        }

        current = current->next;
    }
    while (current != first);
}

/* Change the current directory */

void changeDirectory(const char *name)
{
    if (strcmp(name, "/") == 0)
    {
        cwd = root;
        return;
    }

    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }

        return;
    }

    if (strcmp(name, ".") == 0)
    {
        return;
    }

    FileNode *directory = findChild(cwd, name);

    if (directory == NULL || !directory->isDirectory)
    {
        printf("Directory not found.\n");
        return;
    }

    cwd = directory;
}

/* Print the complete directory path */

void printPath(FileNode *directory)
{
    if (directory == root)
    {
        printf("/");
        return;
    }

    printPath(directory->parent);

    if (directory->parent != root)
    {
        printf("/");
    }

    printf("%s", directory->name);
}

/* Display virtual disk usage */

void showDiskUsage(void)
{
    int used = NUM_BLOCKS - freeCount;

    printf("Total blocks: %d\n", NUM_BLOCKS);
    printf("Used blocks : %d\n", used);
    printf("Free blocks : %d\n", freeCount);

    printf("Disk usage  : %.2f%%\n", used * 100.0 / NUM_BLOCKS);
}

/* Free all files and directories recursively */

void freeTree(FileNode *node)
{
    while (node->children != NULL)
    {
        FileNode *child = node->children;

        removeChild(node, child);
        freeTree(child);
    }

    if (!node->isDirectory)
    {
        releaseFileBlocks(node);
    }

    free(node);
}

/* Initialize the filesystem */

int initialize(void)
{
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        returnBlock(i);
    }

    root = calloc(1, sizeof(FileNode));

    if (root == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }

    strcpy(root->name, "/");

    root->isDirectory = 1;
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;

    cwd = root;

    return 1;
}

/* Display commands */

void showHelp(void)
{
    printf("\nCommands:\n");
    printf("mkdir name\n");
    printf("create name\n");
    printf("write name \"content\"\n");
    printf("read name\n");
    printf("delete name\n");
    printf("rmdir name\n");
    printf("ls\n");
    printf("cd name\n");
    printf("cd ..\n");
    printf("cd /\n");
    printf("pwd\n");
    printf("df\n");
    printf("help\n");
    printf("exit\n\n");
}

int main(void)
{
    char input[INPUT_SIZE];
    char command[INPUT_SIZE];
    char name[INPUT_SIZE];
    char content[INPUT_SIZE];

    if (!initialize())
    {
        return 1;
    }

    printf("Virtual File System ready.\n");
    showHelp();

    while (1)
    {
        printPath(cwd);
        printf(" > ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        int length = (int)strlen(input);

        /* Remove the newline before extracting words */

        while (length > 0 && (input[length - 1] == '\n' || input[length - 1] == '\r'))
        {
            length--;
            input[length] = '\0';
        }

        int i = 0;
        int j = 0;

        /* Skip spaces before the command */

        while (input[i] == ' ' || input[i] == '\t')
        {
            i++;
        }

        if (input[i] == '\0')
        {
            continue;
        }

        while (input[i] != '\0' &&
               input[i] != ' ' &&
               input[i] != '\t')
        {
            command[j] = input[i];
            j++;
            i++;
        }

        command[j] = '\0';

        /* Skip spaces after the command */

        while (input[i] == ' ' || input[i] == '\t')
        {
            i++;
        }

        /* Handle commands that do not require arguments */

        if (strcmp(command, "ls") == 0 ||
            strcmp(command, "pwd") == 0 ||
            strcmp(command, "df") == 0 ||
            strcmp(command, "help") == 0 ||
            strcmp(command, "exit") == 0)
        {
            if (input[i] != '\0')
            {
                printf("This command does not take arguments.\n");
                continue;
            }

            if (strcmp(command, "ls") == 0)
            {
                listEntries();
            }
            else if (strcmp(command, "pwd") == 0)
            {
                printPath(cwd);
                printf("\n");
            }
            else if (strcmp(command, "df") == 0)
            {
                showDiskUsage();
            }
            else if (strcmp(command, "help") == 0)
            {
                showHelp();
            }
            else
            {
                break;
            }

            continue;
        }

        /* Reject unknown commands */

        if (strcmp(command, "mkdir") != 0 &&
            strcmp(command, "create") != 0 &&
            strcmp(command, "read") != 0 &&
            strcmp(command, "delete") != 0 &&
            strcmp(command, "rmdir") != 0 &&
            strcmp(command, "cd") != 0 &&
            strcmp(command, "write") != 0)
        {
            printf("Unknown command. Type 'help'.\n");
            continue;
        }

        /* Check whether a name was provided */

        if (input[i] == '\0')
        {
            printf("Provide a file or directory name.\n");
            continue;
        }

        /* Read the second word: file or directory name */

        j = 0;

        while (input[i] != '\0' &&
               input[i] != ' ' &&
               input[i] != '\t')
        {
            name[j] = input[i];
            j++;
            i++;
        }

        name[j] = '\0';

        if (j >= NAME_SIZE)
        {
            printf("Name cannot exceed 50 characters.\n");
            continue;
        }

        /* Skip spaces after the name */

        while (input[i] == ' ' || input[i] == '\t')
        {
            i++;
        }

        /* For write, read the remaining text as content */

        if (strcmp(command, "write") == 0)
        {
            if (input[i] == '\0')
            {
                printf("Usage: write name \"content\"\n");
                continue;
            }

            int end = length;

            if (input[i] == '"' || input[i] == '\'')
            {
                char quote = input[i];

                /* Ignore spaces after the closing quote */

                while (end > i &&
                       (input[end - 1] == ' ' ||
                        input[end - 1] == '\t'))
                {
                    end--;
                }

                if (end - i < 2 || input[end - 1] != quote)
                {
                    printf("Missing closing quote.\n");
                    continue;
                }

                i++;
                end--;
            }

            j = 0;

            while (i < end)
            {
                content[j] = input[i];
                j++;
                i++;
            }

            content[j] = '\0';

            writeFile(name, content);
            continue;
        }

        if (strcmp(command, "mkdir") == 0)
        {
            createEntry(name, 1);
        }
        else if (strcmp(command, "create") == 0)
        {
            createEntry(name, 0);
        }
        else if (strcmp(command, "read") == 0)
        {
            readFile(name);
        }
        else if (strcmp(command, "delete") == 0)
        {
            deleteFile(name);
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            removeDirectory(name);
        }
        else if (strcmp(command, "cd") == 0)
        {
            changeDirectory(name);
        }
    }

    freeTree(root);

    root = NULL;
    cwd = NULL;
    freeHead = NULL;
    freeTail = NULL;
    freeCount = 0;

    printf("Memory released. Exiting...\n");

    return 0;
}