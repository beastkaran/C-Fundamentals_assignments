
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Block{
    int start;
    int size;
    int id;
    int status;
};

int ensurecapacity(struct Block **blocks,int *capacity,int required){
    if (required<=*capacity){
        return 1;
    }

    int newCapacity=*capacity*2;

    while (newCapacity<required){
        newCapacity*=2;
    }

    struct Block *temp=realloc(*blocks,newCapacity*sizeof(struct Block));

    if (temp==NULL){
        return 0;
    }

    *blocks=temp;
    *capacity=newCapacity;

    return 1;
}

int findblock(struct Block blocks[],int blockcount,int request,int strategy){
    int index=-1;

    for (int i=0;i<blockcount;i++){
        if (blocks[i].status==0 && blocks[i].size>=request){
            if (strategy==1){
                return i;
            }
            else if (strategy==2){
                if (index==-1 || blocks[i].size<blocks[index].size){
                    index=i;
                }
            }
            else if (strategy==3){
                if (index==-1 || blocks[i].size>blocks[index].size){
                    index=i;
                }
            }
        }
    }

    return index;
}

int allocateatindex(struct Block **blocks,int *blockcount,int *capacity,int index,int request,int id){
    int oldStart=(*blocks)[index].start;
    int oldSize=(*blocks)[index].size;

    if (oldSize>request){
        if (!ensurecapacity(blocks,capacity,*blockcount+1)){
            return 0;
        }

        for (int i=*blockcount;i>index+1;i--){
            (*blocks)[i]=(*blocks)[i-1];
        }

        (*blocks)[index+1].start=oldStart+request;
        (*blocks)[index+1].size=oldSize-request;
        (*blocks)[index+1].id=-1;
        (*blocks)[index+1].status=0;

        (*blockcount)++;
    }

    (*blocks)[index].start=oldStart;
    (*blocks)[index].size=request;
    (*blocks)[index].id=id;
    (*blocks)[index].status=1;

    return 1;
}

void allocatememory(struct Block **blocks,int *blockcount,int *capacity,int *nextId,int totalsize,int request,int strategy){
    if (request<=0 || request>totalsize){
        printf("Invalid allocation size.\n");
        return;
    }

    int index=findblock(*blocks,*blockcount,request,strategy);

    if (index==-1){
        int totalFree=0;

        printf("Allocation failed: no contiguous free block is large enough.\n");
        printf("Individual free blocks: ");

        for (int i=0;i<*blockcount;i++){
            if ((*blocks)[i].status==0){
                printf("%d ",(*blocks)[i].size);
                totalFree+=(*blocks)[i].size;
            }
        }

        printf("\nTotal free memory: %d bytes\n",totalFree);

        if (totalFree>=request){
            printf("External fragmentation detected.\n");
        }

        return;
    }

    int id=*nextId;

    if (!allocateatindex(blocks,blockcount,capacity,index,request,id)){
        printf("Allocation failed: metadata memory unavailable.\n");
        return;
    }

    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n",id,(*blocks)[index].start,request);

    (*nextId)++;
}

int findallocation(struct Block blocks[],int blockcount,int id){
    for (int i=0;i<blockcount;i++){
        if (blocks[i].status==1 && blocks[i].id==id){
            return i;
        }
    }

    return -1;
}

void removeblock(struct Block blocks[],int *blockcount,int index){
    for (int i=index;i<*blockcount-1;i++){
        blocks[i]=blocks[i+1];
    }

    (*blockcount)--;
}

void mergeblocks(struct Block blocks[],int *blockcount){
    int i=0;

    while (i<*blockcount-1){
        if (blocks[i].status==0 && blocks[i+1].status==0){
            blocks[i].size+=blocks[i+1].size;

            removeblock(blocks,blockcount,i+1);
        }
        else{
            i++;
        }
    }
}

void releaseatindex(struct Block blocks[],int *blockcount,int index){
    blocks[index].status=0;
    blocks[index].id=-1;

    mergeblocks(blocks,blockcount);
}

void releasememory(struct Block blocks[],int *blockcount,int id){
    int index=findallocation(blocks,*blockcount,id);

    if (index==-1){
        printf("Invalid allocation ID.\n");
        return;
    }

    releaseatindex(blocks,blockcount,index);

    printf("Allocation released: %d\n",id);
}

int findbystart(struct Block blocks[],int blockcount,int start){
    for (int i=0;i<blockcount;i++){
        if (blocks[i].start==start && blocks[i].status==1){
            return i;
        }
    }

    return -1;
}

void resizememory(struct Block **blocks,int *blockcount,int *capacity,int id,int newSize,int totalsize,int strategy,char *memory){
    int index=findallocation(*blocks,*blockcount,id);

    if (index==-1){
        printf("Invalid allocation ID.\n");
        return;
    }

    if (newSize<=0 || newSize>totalsize){
        printf("Invalid resize size.\n");
        return;
    }

    int oldSize=(*blocks)[index].size;
    int oldStart=(*blocks)[index].start;

    if (newSize==oldSize){
        printf("Allocation size is already the same.\n");
        return;
    }

    if (newSize<oldSize){
        if (!ensurecapacity(blocks,capacity,*blockcount+1)){
            printf("Resize failed: metadata memory unavailable.\n");
            return;
        }

        for (int i=*blockcount;i>index+1;i--){
            (*blocks)[i]=(*blocks)[i-1];
        }

        (*blocks)[index].size=newSize;

        (*blocks)[index+1].start=oldStart+newSize;
        (*blocks)[index+1].size=oldSize-newSize;
        (*blocks)[index+1].id=-1;
        (*blocks)[index+1].status=0;

        (*blockcount)++;

        mergeblocks(*blocks,blockcount);

        printf("Allocation resized successfully.\n");
        return;
    }

    int extra=newSize-oldSize;

    if (index+1<*blockcount && (*blocks)[index+1].status==0 && (*blocks)[index+1].size>=extra){
        (*blocks)[index].size=newSize;

        if ((*blocks)[index+1].size==extra){
            removeblock(*blocks,blockcount,index+1);
        }
        else{
            (*blocks)[index+1].start+=extra;
            (*blocks)[index+1].size-=extra;
        }

        printf("Allocation expanded in place.\n");
        return;
    }

    int newIndex=findblock(*blocks,*blockcount,newSize,strategy);

    if (newIndex==-1){
        printf("Resize failed: no suitable free block.\n");
        printf("Original allocation remains unchanged.\n");
        return;
    }

    int newStart=(*blocks)[newIndex].start;

    if (!allocateatindex(blocks,blockcount,capacity,newIndex,newSize,id)){
        printf("Resize failed: metadata memory unavailable.\n");
        return;
    }

    memmove(memory+newStart,memory+oldStart,oldSize);

    int oldIndex=findbystart(*blocks,*blockcount,oldStart);

    if (oldIndex!=-1){
        releaseatindex(*blocks,blockcount,oldIndex);
    }

    printf("Allocation moved and resized successfully.\n");
}

void displayMap(struct Block blocks[],int blockcount,int totalsize){
    printf("\nMemory Map (%d bytes):\n",totalsize);

    for (int i=0;i<blockcount;i++){
        if (blocks[i].status==0){
            printf("[Free: %d bytes, Start: %d]\n",blocks[i].size,blocks[i].start);
        }
        else{
            printf("[Allocated #%d: %d bytes, Start: %d]\n",blocks[i].id,blocks[i].size,blocks[i].start);
        }
    }
}

void displayStats(struct Block blocks[],int blockcount,int totalsize){
    int allocated=0;
    int freeMemory=0;
    int allocatedBlocks=0;
    int freeBlocks=0;
    int largestFree=0;

    for (int i=0;i<blockcount;i++){
        if (blocks[i].status==1){
            allocated+=blocks[i].size;
            allocatedBlocks++;
        }
        else{
            freeMemory+=blocks[i].size;
            freeBlocks++;

            if (blocks[i].size>largestFree){
                largestFree=blocks[i].size;
            }
        }
    }

    printf("\nTotal memory: %d bytes\n",totalsize);
    printf("Allocated: %d bytes\n",allocated);
    printf("Free: %d bytes\n",freeMemory);
    printf("Allocated blocks: %d\n",allocatedBlocks);
    printf("Free blocks: %d\n",freeBlocks);
    printf("Largest free block: %d bytes\n",largestFree);
}

int main(){
    char *memory;
    struct Block *blocks;

    int totalsize;
    int blockcount=1;
    int capacity=2;
    int nextId=1;
    int strategy;
    int choice;

    printf("Enter memory pool size: ");

    if (scanf("%d",&totalsize)!=1 || totalsize<1 || totalsize>1000000){
        printf("Invalid pool size.\n");
        return 0;
    }

    memory=malloc(totalsize*sizeof(char));

    if (memory==NULL){
        printf("Memory allocation failed.\n");
        return 0;
    }

    blocks=malloc(capacity*sizeof(struct Block));

    if (blocks==NULL){
        printf("Metadata allocation failed.\n");
        free(memory);
        return 0;
    }

    blocks[0].start=0;
    blocks[0].size=totalsize;
    blocks[0].id=-1;
    blocks[0].status=0;

    printf("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");

    if (scanf("%d",&strategy)!=1 || strategy<1 || strategy>3){
        printf("Invalid strategy.\n");
        free(memory);
        free(blocks);
        return 0;
    }

    while (1){
        printf("\nMenu:\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        printf("Choice: ");

        if (scanf("%d",&choice)!=1){
            printf("Invalid input.\n");
            break;
        }

        if (choice==1){
            int request;

            printf("Requested size: ");

            if (scanf("%d",&request)!=1){
                printf("Invalid input.\n");
                break;
            }

            allocatememory(&blocks,&blockcount,&capacity,&nextId,totalsize,request,strategy);
        }

        else if (choice==2){
            int id;

            printf("Allocation ID: ");

            if (scanf("%d",&id)!=1){
                printf("Invalid input.\n");
                break;
            }

            releasememory(blocks,&blockcount,id);
        }

        else if (choice==3){
            int id,newSize;

            printf("Allocation ID: ");

            if (scanf("%d",&id)!=1){
                printf("Invalid input.\n");
                break;
            }

            printf("New size: ");

            if (scanf("%d",&newSize)!=1){
                printf("Invalid input.\n");
                break;
            }

            resizememory(&blocks,&blockcount,&capacity,id,newSize,totalsize,strategy,memory);
        }

        else if (choice==4){
            displayMap(blocks,blockcount,totalsize);
        }

        else if (choice==5){
            displayStats(blocks,blockcount,totalsize);
        }

        else if (choice==6){
            printf("Select strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");

            if (scanf("%d",&strategy)!=1 || strategy<1 || strategy>3){
                printf("Invalid strategy.\n");
                break;
            }

            printf("Strategy changed successfully.\n");
        }

        else if (choice==7){
            break;
        }

        else{
            printf("Invalid choice.\n");
        }
    }

    free(memory);
    free(blocks);

    return 0;
}
