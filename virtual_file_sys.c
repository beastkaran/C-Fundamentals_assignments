
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode
{
    char name[MAX_NAME + 1];
    int isDir;

    struct FileNode *parent;
    struct FileNode *child;

    struct FileNode *next;
    struct FileNode *prev;

    int *blockPointers;
    int blockCount;
    int contentSize;

} FileNode;

unsigned char **virtualDisk;

FreeBlock *freeHead=NULL;
FreeBlock *freeTail=NULL;

FileNode *root=NULL;
FileNode *cwd=NULL;

int freeCount=0;


// Initialize the free block list
void initializeFreeList()
{
    for (int i = 0; i<NUM_BLOCKS; i++)
    {
        FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));

        newBlock->index=i;
        newBlock->next=NULL;
        newBlock->prev=freeTail;

        if (freeHead==NULL)
        {
            freeHead=newBlock;
            freeTail=newBlock;
        }
        else
        {
            freeTail->next=newBlock;
            freeTail=newBlock;
        }

        freeCount++;
    }
}


// Initialize virtual disk and root directory
void initializeSystem()
{
    virtualDisk = (unsigned char **)malloc(NUM_BLOCKS * sizeof(unsigned char *));

    for (int i = 0;i<NUM_BLOCKS; i++)
    {
        virtualDisk[i] = (unsigned char *)calloc(BLOCK_SIZE, sizeof(unsigned char));
    }

    initializeFreeList();

    root = (FileNode *)malloc(sizeof(FileNode));

    strcpy(root->name, "/");

    root->isDir = 1;
    root->parent = NULL;
    root->child = NULL;

    root->next=root;
    root->prev=root;

    root->blockPointers=NULL;
    root->blockCount=0;
    root->contentSize=0;

    cwd=root;
}


// Find a file or directory in the current directory
FileNode *findChild(FileNode *dir, char name[])
{
    if (dir->child==NULL)
    {
        return NULL;
    }

    FileNode *temp = dir->child;

    do
    {
        if (strcmp(temp->name, name)==0)
        {
            return temp;
        }

        temp=temp->next;

    } while (temp!= dir->child);

    return NULL;
}


// Insert a node into circular doubly linked list
void addChild(FileNode *dir, FileNode *newNode)
{
    newNode->parent = dir;

    if (dir->child== NULL)
    {
        dir->child=newNode;

        newNode->next=newNode;
        newNode->prev=newNode;
    }
    else
    {
        FileNode *last = dir->child->prev;

        last->next = newNode;
        newNode->prev = last;

        newNode->next=dir->child;
        dir->child->prev=newNode;
    }
}


// Remove a node from circular doubly linked list
void removeChild(FileNode *dir, FileNode *node)
{
    if (node->next == node)
    {
        dir->child = NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (dir->child  ==node)
        {
            dir->child=node->next;
        }
    }

    node->next = NULL;
    node->prev = NULL;
}


// Insert a free block at the end of free list
void addFreeBlock(int index)
{
    FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));

    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freeTail;

    if (freeHead == NULL)
    {
        freeHead = newBlock;
        freeTail = newBlock;
    }
    else
    {
        freeTail->next = newBlock;
        freeTail = newBlock;
    }

    freeCount++;
}


// Remove a free block from the beginning
int getFreeBlock()
{
    if (freeHead == NULL)
    {
        return -1;
    }

    FreeBlock *temp = freeHead;
    int index = temp->index;

    freeHead = freeHead->next;

    if (freeHead == NULL)
    {
        freeTail = NULL;
    }
    else
    {
        freeHead->prev = NULL;
    }

    free(temp);
    freeCount--;

    return index;
}


// Return all blocks of a file to free list
void releaseFileBlocks(FileNode *file)
{
    for (int i = 0; i < file->blockCount; i++)
    {
        addFreeBlock(file->blockPointers[i]);
    }

    free(file->blockPointers);

    file->blockPointers = NULL;
    file->blockCount = 0;
    file->contentSize = 0;
}


// Create a directory
void mkdir(char name[])
{
    if (findChild(cwd, name) != NULL)
    {
        printf("Name already exists!\n");
        return;
    }

    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));

    strcpy(newNode->name, name);

    newNode->isDir = 1;
    newNode->parent = cwd;
    newNode->child = NULL;

    newNode->next = NULL;
    newNode->prev = NULL;

    newNode->blockPointers = NULL;
    newNode->blockCount = 0;
    newNode->contentSize = 0;

    addChild(cwd, newNode);

    printf("Directory '%s' created successfully.\n", name);
}


// Create a file
void create(char name[])
{
    if (findChild(cwd, name) != NULL)
    {
        printf("Name already exists!\n");
        return;
    }

    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));

    strcpy(newNode->name, name);

    newNode->isDir = 0;
    newNode->parent = cwd;
    newNode->child = NULL;

    newNode->next = NULL;
    newNode->prev = NULL;

    newNode->blockPointers = NULL;
    newNode->blockCount = 0;
    newNode->contentSize = 0;

    addChild(cwd, newNode);

    printf("File '%s' created successfully.\n", name);
}


// Write content into a file
void writeFile(char name[], char content[])
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDir == 1)
    {
        printf("File not found!\n");
        return;
    }

    int size = strlen(content);

    int neededBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Check whether enough blocks are available
    if (freeCount + file->blockCount < neededBlocks)
    {
        printf("Disk is full! Cannot write file.\n");
        return;
    }

    int *newPointers = NULL;

    if (neededBlocks > 0)
    {
        newPointers = (int *)malloc(neededBlocks * sizeof(int));

        if (newPointers == NULL)
        {
            printf("Memory allocation failed!\n");
            return;
        }
    }

    // Release previously allocated blocks
    releaseFileBlocks(file);

    // Allocate required blocks from free list
    for (int i = 0; i < neededBlocks; i++)
    {
        newPointers[i] = getFreeBlock();
    }

    file->blockPointers = newPointers;
    file->blockCount = neededBlocks;
    file->contentSize = size;

    // Write content into virtual disk
    int pos = 0;

    for (int i = 0; i < neededBlocks; i++)
    {
        int block = file->blockPointers[i];

        memset(virtualDisk[block], 0, BLOCK_SIZE);

        for (int j = 0; j < BLOCK_SIZE && pos < size; j++)
        {
            virtualDisk[block][j] = content[pos];
            pos++;
        }
    }

    printf("Data written successfully (size=%d bytes).\n", size);
}


// Read content from a file
void readFile(char name[])
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDir == 1)
    {
        printf("File not found!\n");
        return;
    }

    if (file->blockCount == 0)
    {
        printf("File is empty.\n");
        return;
    }

    int count = 0;

    // Read data from allocated blocks
    for (int i = 0; i < file->blockCount; i++)
    {
        int block = file->blockPointers[i];

        for (int j = 0; j < BLOCK_SIZE && count < file->contentSize; j++)
        {
            putchar(virtualDisk[block][j]);
            count++;
        }
    }

    printf("\n");
}


// Delete a file
void deleteFile(char name[])
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDir == 1)
    {
        printf("File not found!\n");
        return;
    }

    // Return allocated blocks to free list
    releaseFileBlocks(file);

    // Remove file from directory
    removeChild(cwd, file);

    free(file);

    printf("File deleted successfully.\n");
}


// Remove an empty directory
void rmdir(char name[])
{
    FileNode *dir = findChild(cwd, name);

    if (dir == NULL || dir->isDir == 0)
    {
        printf("Directory not found!\n");
        return;
    }

    if (dir->child != NULL)
    {
        printf("Directory is not empty!\n");
        return;
    }

    removeChild(cwd, dir);

    free(dir);

    printf("Directory removed successfully.\n");
}


// List files and directories
void ls()
{
    if (cwd->child == NULL)
    {
        printf("(empty)\n");
        return;
    }

    FileNode *temp = cwd->child;

    do
    {
        printf("%s", temp->name);

        if (temp->isDir == 1)
        {
            printf("/");
        }

        printf("\n");

        temp = temp->next;

    } while (temp != cwd->child);
}


// Change current working directory
void cd(char name[])
{
    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }
        else
        {
            printf("Already in root directory.\n");
            return;
        }
    }
    else if (strcmp(name, "/") == 0)
    {
        cwd = root;
    }
    else
    {
        FileNode *dir = findChild(cwd, name);

        if (dir == NULL || dir->isDir == 0)
        {
            printf("Directory not found!\n");
            return;
        }

        cwd = dir;
    }

    printf("Moved to ");

    char path[1024];
    getPath(cwd, path);

    printf("%s\n", path);
}


// Build current directory path
void getPath(FileNode *dir, char path[])
{
    if (dir == root)
    {
        strcpy(path, "/");
        return;
    }

    getPath(dir->parent, path);

    if (strcmp(path, "/") != 0)
    {
        strcat(path, "/");
    }

    strcat(path, dir->name);
}


// Display current working directory
void pwd()
{
    char path[1024];

    getPath(cwd, path);

    printf("%s\n", path);
}


// Display disk usage
void df()
{
    int usedBlocks = NUM_BLOCKS - freeCount;

    float usage = ((float)usedBlocks / NUM_BLOCKS) * 100;

    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", usage);
}


// Free all directory and file nodes
void freeTree(FileNode *dir)
{
    while (dir->child != NULL)
    {
        FileNode *temp = dir->child;

        removeChild(dir, temp);

        if (temp->isDir == 1)
        {
            freeTree(temp);
        }
        else
        {
            free(temp->blockPointers);
        }

        free(temp);
    }
}


// Free the complete free block list
void freeFreeList()
{
    FreeBlock *temp = freeHead;

    while (temp != NULL)
    {
        FreeBlock *next = temp->next;

        free(temp);

        temp = next;
    }

    freeHead = NULL;
    freeTail = NULL;
}


// Release all allocated memory
void exitSystem()
{
    freeTree(root);
    free(root);

    freeFreeList();

    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        free(virtualDisk[i]);
    }

    free(virtualDisk);

    printf("Memory released. Exiting program...\n");
}


// Main function
int main()
{
    initializeSystem();

    char line[4096];

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1)
    {
        char path[1024];

        getPath(cwd, path);

        printf("%s > ", path);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        char *cmd = strtok(line, " \t");

        if (cmd == NULL)
        {
            continue;
        }

        char *arg = strtok(NULL, " \t");

        if (strcmp(cmd, "mkdir") == 0)
        {
            if (arg != NULL)
            {
                mkdir(arg);
            }
            else
            {
                printf("Enter directory name.\n");
            }
        }
        else if (strcmp(cmd, "create") == 0)
        {
            if (arg != NULL)
            {
                create(arg);
            }
            else
            {
                printf("Enter file name.\n");
            }
        }
        else if (strcmp(cmd, "write") == 0)
        {
            if (arg != NULL)
            {
                char *content = strtok(NULL, "\n");

                if (content == NULL)
                {
                    content = "";
                }

                // Remove leading spaces
                while (*content == ' ' || *content == '\t')
                {
                    content++;
                }

                // Remove quotation marks if present
                int len = strlen(content);

                if (len >= 2 && content[0] == '"' && content[len - 1] == '"')
                {
                    content[len - 1] = '\0';
                    content++;
                }

                writeFile(arg, content);
            }
            else
            {
                printf("Enter file name and content.\n");
            }
        }
        else if (strcmp(cmd, "read") == 0)
        {
            if (arg != NULL)
            {
                readFile(arg);
            }
            else
            {
                printf("Enter file name.\n");
            }
        }
        else if (strcmp(cmd, "delete") == 0)
        {
            if (arg != NULL)
            {
                deleteFile(arg);
            }
            else
            {
                printf("Enter file name.\n");
            }
        }
        else if (strcmp(cmd, "rmdir") == 0)
        {
            if (arg != NULL)
            {
                rmdir(arg);
            }
            else
            {
                printf("Enter directory name.\n");
            }
        }
        else if (strcmp(cmd, "ls") == 0)
        {
            ls();
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            if (arg != NULL)
            {
                cd(arg);
            }
            else
            {
                printf("Enter directory name.\n");
            }
        }
        else if (strcmp(cmd, "pwd") == 0)
        {
            pwd();
        }
        else if (strcmp(cmd, "df") == 0)
        {
            df();
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            exitSystem();
            break;
        }
        else
        {
            printf("Invalid command!\n");
        }
    }

    return 0;
}
