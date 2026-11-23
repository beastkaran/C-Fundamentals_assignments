
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define TEAM_COUNT 10
#define MAX_NAME 51

// My own player structure
struct PlayerNode
{
    int id;
    char name[MAX_NAME];
    int role;

    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;

    struct PlayerNode *nextTeam;
    struct PlayerNode *nextRole;
};

// Team structure
struct Team
{
    int id;
    char name[MAX_NAME];

    int totalPlayers;

    float totalStrikeRate;
    int battingCount;
    float avgStrikeRate;

    struct PlayerNode *head;
    struct PlayerNode *tail;

    struct PlayerNode *roleHead[3];
};

// Used for merging sorted player lists
struct HeapItem
{
    struct PlayerNode *player;
    int teamIndex;
};

struct Team teamsData[TEAM_COUNT];


// Clear input buffer
void clearInput()
{
    while (getchar()!='\n');
}


// Read a string
void readString(char str[])
{
    fgets(str,MAX_NAME,stdin);
    str[strcspn(str,"\n")]='\0';
}


// Convert role number into role name
char *getRoleName(int role)
{
    if (role==1)
    {
        return "Batsman";
    }
    else if (role==2)
    {
        return "Bowler";
    }
    else
    {
        return "All-rounder";
    }
}


// Calculate performance index
float calculateIndex(int role,float avg,float sr,int wickets,float economy)
{
    if (role==1)
    {
        return (avg*sr)/100;
    }
    else if (role==2)
    {
        return (wickets*2)+(100-economy);
    }
    else
    {
        return ((avg*sr)/100)+(wickets*2);
    }
}


// Binary search for team using Team ID
int searchTeam(int id)
{
    int low=0;
    int high=TEAM_COUNT-1;

    while (low<=high)
    {
        int mid=(low+high)/2;

        if (teamsData[mid].id==id)
        {
            return mid;
        }
        else if (teamsData[mid].id<id)
        {
            low=mid+1;
        }
        else
        {
            high=mid-1;
        }
    }

    return -1;
}


// Find team using its name
int findTeamByName(char name[])
{
    for (int i=0;i<TEAM_COUNT;i++)
    {
        if (strcmp(teamsData[i].name,name)==0)
        {
            return i;
        }
    }

    return -1;
}


// Check whether player ID already exists
int checkPlayerId(int id)
{
    for (int i=0;i<TEAM_COUNT;i++)
    {
        struct PlayerNode *temp=teamsData[i].head;

        while (temp!=NULL)
        {
            if (temp->id==id)
            {
                return 1;
            }
            temp=temp->nextTeam;
        }
    }

    return 0;
}


// Insert player into role-wise sorted linked list
void insertRoleList(int teamIndex,struct PlayerNode *newPlayer)
{
    int role=newPlayer->role-1;

    struct PlayerNode *temp=teamsData[teamIndex].roleHead[role];

    // Insert at beginning
    if (temp==NULL || newPlayer->performanceIndex>temp->performanceIndex)
    {
        newPlayer->nextRole=temp;
        teamsData[teamIndex].roleHead[role]=newPlayer;
        return;
    }

    // Find correct position
    while (temp->nextRole!=NULL &&
           temp->nextRole->performanceIndex>=newPlayer->performanceIndex)
    {
        temp=temp->nextRole;
    }

    newPlayer->nextRole=temp->nextRole;
    temp->nextRole=newPlayer;
}


// Add player to team linked list
void insertPlayer(int teamIndex,struct PlayerNode *newPlayer)
{
    if (teamsData[teamIndex].head==NULL)
    {
        teamsData[teamIndex].head=newPlayer;
        teamsData[teamIndex].tail=newPlayer;
    }
    else
    {
        teamsData[teamIndex].tail->nextTeam=newPlayer;
        teamsData[teamIndex].tail=newPlayer;
    }

    newPlayer->nextTeam=NULL;
    newPlayer->nextRole=NULL;

    // Update team count
    teamsData[teamIndex].totalPlayers++;

    // Update average batting strike rate
    if (newPlayer->role==1 || newPlayer->role==3)
    {
        teamsData[teamIndex].totalStrikeRate+=newPlayer->strikeRate;
        teamsData[teamIndex].battingCount++;

        teamsData[teamIndex].avgStrikeRate=
        teamsData[teamIndex].totalStrikeRate/teamsData[teamIndex].battingCount;
    }

    // Insert into role-wise sorted list
    insertRoleList(teamIndex,newPlayer);
}


// Initialize teams and players from header file
void initializeData()
{
    // Team ID mapping
    int mapping[TEAM_COUNT]={4,1,3,6,5,7,2,8,0,9};

    for (int i=0;i<TEAM_COUNT;i++)
    {
        teamsData[i].id=i+1;
        strcpy(teamsData[i].name,teams[mapping[i]]);

        teamsData[i].totalPlayers=0;
        teamsData[i].totalStrikeRate=0;
        teamsData[i].battingCount=0;
        teamsData[i].avgStrikeRate=0;

        teamsData[i].head=NULL;
        teamsData[i].tail=NULL;

        for (int j=0;j<3;j++)
        {
            teamsData[i].roleHead[j]=NULL;
        }
    }

    // Load players from predefined data
    for (int i=0;i<playerCount;i++)
    {
        int teamIndex=findTeamByName((char *)players[i].team);

        if (teamIndex==-1)
        {
            continue;
        }

        struct PlayerNode *newPlayer=
        (struct PlayerNode *)malloc(sizeof(struct PlayerNode));

        if (newPlayer==NULL)
        {
            printf("Memory allocation failed!\n");
            exit(1);
        }

        newPlayer->id=players[i].id;

        strcpy(newPlayer->name,players[i].name);

        if (strcmp(players[i].role,"Batsman")==0)
        {
            newPlayer->role=1;
        }
        else if (strcmp(players[i].role,"Bowler")==0)
        {
            newPlayer->role=2;
        }
        else
        {
            newPlayer->role=3;
        }

        newPlayer->totalRuns=players[i].totalRuns;
        newPlayer->battingAverage=players[i].battingAverage;
        newPlayer->strikeRate=players[i].strikeRate;
        newPlayer->wickets=players[i].wickets;
        newPlayer->economyRate=players[i].economyRate;

        newPlayer->performanceIndex=
        calculateIndex(newPlayer->role,
                       newPlayer->battingAverage,
                       newPlayer->strikeRate,
                       newPlayer->wickets,
                       newPlayer->economyRate);

        insertPlayer(teamIndex,newPlayer);
    }
}


// Display one player's details
void displayPlayer(struct PlayerNode *p)
{
    printf("%-6d %-22s %-13s %-8d %-8.2f %-8.2f %-7d %-8.2f %-10.2f\n",
           p->id,
           p->name,
           getRoleName(p->role),
           p->totalRuns,
           p->battingAverage,
           p->strikeRate,
           p->wickets,
           p->economyRate,
           p->performanceIndex);
}


// Display all players of a team
void displayTeam()
{
    int id;

    printf("Enter Team ID: ");
    scanf("%d",&id);

    int index=searchTeam(id);

    if (index==-1)
    {
        printf("Invalid Team ID!\n");
        return;
    }

    printf("\nPlayers of Team %s:\n",teamsData[index].name);

    printf("-----------------------------------------------------------------------------------------------\n");

    printf("%-6s %-22s %-13s %-8s %-8s %-8s %-7s %-8s %-10s\n",
           "ID","Name","Role","Runs","Avg","SR","Wkts","ER","Perf.Index");

    printf("-----------------------------------------------------------------------------------------------\n");

    struct PlayerNode *temp=teamsData[index].head;

    while (temp!=NULL)
    {
        displayPlayer(temp);
        temp=temp->nextTeam;
    }

    printf("-----------------------------------------------------------------------------------------------\n");

    printf("Total Players: %d\n",teamsData[index].totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",teamsData[index].avgStrikeRate);
}


// Display teams according to average batting strike rate
void displayTeams()
{
    struct Team tempTeams[TEAM_COUNT];

    // Copy team information
    for (int i=0;i<TEAM_COUNT;i++)
    {
        tempTeams[i]=teamsData[i];
    }

    // Sort in descending order
    for (int i=0;i<TEAM_COUNT-1;i++)
    {
        for (int j=0;j<TEAM_COUNT-i-1;j++)
        {
            if (tempTeams[j].avgStrikeRate<tempTeams[j+1].avgStrikeRate)
            {
                struct Team temp=tempTeams[j];
                tempTeams[j]=tempTeams[j+1];
                tempTeams[j+1]=temp;
            }
        }
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");

    printf("----------------------------------------------------------\n");

    printf("%-6s %-22s %-16s %-15s\n",
           "ID","Team Name","Avg Bat SR","Total Players");

    printf("----------------------------------------------------------\n");

    for (int i=0;i<TEAM_COUNT;i++)
    {
        printf("%-6d %-22s %-16.2f %-15d\n",
               tempTeams[i].id,
               tempTeams[i].name,
               tempTeams[i].avgStrikeRate,
               tempTeams[i].totalPlayers);
    }

    printf("----------------------------------------------------------\n");
}


// Display top K players of a specific team and role
void displayTopK()
{
    int id;
    int role;
    int k;

    printf("Enter Team ID: ");
    scanf("%d",&id);

    int index=searchTeam(id);

    if (index==-1)
    {
        printf("Invalid Team ID!\n");
        return;
    }

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d",&role);

    if (role<1 || role>3)
    {
        printf("Invalid role!\n");
        return;
    }

    printf("Enter number of players: ");
    scanf("%d",&k);

    if (k<=0)
    {
        printf("Invalid number of players!\n");
        return;
    }

    struct PlayerNode *temp=teamsData[index].roleHead[role-1];

    if (temp==NULL)
    {
        printf("No players found for this role.\n");
        return;
    }

    printf("\nTop %d %s of Team %s:\n",k,getRoleName(role),teamsData[index].name);

    printf("-----------------------------------------------------------------------------------------------\n");

    printf("%-6s %-22s %-13s %-8s %-8s %-8s %-7s %-8s %-10s\n",
           "ID","Name","Role","Runs","Avg","SR","Wkts","ER","Perf.Index");

    printf("-----------------------------------------------------------------------------------------------\n");

    int count=0;

    // Already sorted, so only K nodes need to be visited
    while (temp!=NULL && count<k)
    {
        displayPlayer(temp);

        temp=temp->nextRole;
        count++;
    }

    printf("-----------------------------------------------------------------------------------------------\n");
}


// Compare two heap elements
int isGreater(struct HeapItem a,struct HeapItem b)
{
    return a.player->performanceIndex>b.player->performanceIndex;
}


// Insert an element into max heap
void heapPush(struct HeapItem heap[],int *size,struct HeapItem item)
{
    int i=*size;

    heap[i]=item;
    (*size)++;

    while (i>0)
    {
        int parent=(i-1)/2;

        if (isGreater(heap[parent],heap[i]))
        {
            break;
        }

        struct HeapItem temp=heap[parent];
        heap[parent]=heap[i];
        heap[i]=temp;

        i=parent;
    }
}


// Remove maximum element from heap
struct HeapItem heapPop(struct HeapItem heap[],int *size)
{
    struct HeapItem result=heap[0];

    (*size)--;

    heap[0]=heap[*size];

    int i=0;

    while (1)
    {
        int left=2*i+1;
        int right=2*i+2;
        int largest=i;

        if (left<*size && isGreater(heap[left],heap[largest]))
        {
            largest=left;
        }

        if (right<*size && isGreater(heap[right],heap[largest]))
        {
            largest=right;
        }

        if (largest==i)
        {
            break;
        }

        struct HeapItem temp=heap[i];
        heap[i]=heap[largest];
        heap[largest]=temp;

        i=largest;
    }

    return result;
}


// Display all players of a role across all teams
void displayAllRole()
{
    int role;

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d",&role);

    if (role<1 || role>3)
    {
        printf("Invalid role!\n");
        return;
    }

    struct HeapItem heap[TEAM_COUNT];
    int heapSize=0;

    // Add first player of each team's sorted role list
    for (int i=0;i<TEAM_COUNT;i++)
    {
        if (teamsData[i].roleHead[role-1]!=NULL)
        {
            struct HeapItem item;

            item.player=teamsData[i].roleHead[role-1];
            item.teamIndex=i;

            heapPush(heap,&heapSize,item);
        }
    }

    printf("\n%s of all teams:\n",getRoleName(role));

    printf("-----------------------------------------------------------------------------------------------------------\n");

    printf("%-6s %-22s %-18s %-13s %-8s %-8s %-8s %-7s %-8s %-10s\n",
           "ID","Name","Team","Role","Runs","Avg","SR","Wkts","ER","Perf.Index");

    printf("-----------------------------------------------------------------------------------------------------------\n");

    // Merge all sorted lists using max heap
    while (heapSize>0)
    {
        struct HeapItem item=heapPop(heap,&heapSize);

        struct PlayerNode *p=item.player;

        printf("%-6d %-22s %-18s %-13s %-8d %-8.2f %-8.2f %-7d %-8.2f %-10.2f\n",
               p->id,
               p->name,
               teamsData[item.teamIndex].name,
               getRoleName(p->role),
               p->totalRuns,
               p->battingAverage,
               p->strikeRate,
               p->wickets,
               p->economyRate,
               p->performanceIndex);

        // Add next player from same team's list
        if (p->nextRole!=NULL)
        {
            struct HeapItem nextItem;

            nextItem.player=p->nextRole;
            nextItem.teamIndex=item.teamIndex;

            heapPush(heap,&heapSize,nextItem);
        }
    }

    printf("-----------------------------------------------------------------------------------------------------------\n");
}


// Add a new player
void addPlayer()
{
    int id;
    int role;
    int teamId;
    int index;

    struct PlayerNode *newPlayer;

    printf("Enter Team ID to add player: ");
    scanf("%d",&teamId);

    index=searchTeam(teamId);

    if (index==-1)
    {
        printf("Invalid Team ID!\n");
        return;
    }

    newPlayer=(struct PlayerNode *)malloc(sizeof(struct PlayerNode));

    if (newPlayer==NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter Player ID: ");
    scanf("%d",&id);

    if (id<1 || id>1000 || checkPlayerId(id))
    {
        printf("Invalid or duplicate Player ID!\n");
        free(newPlayer);
        return;
    }

    newPlayer->id=id;

    clearInput();

    printf("Enter Player Name: ");
    readString(newPlayer->name);

    if (strlen(newPlayer->name)==0)
    {
        printf("Invalid name!\n");
        free(newPlayer);
        return;
    }

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d",&role);

    if (role<1 || role>3)
    {
        printf("Invalid role!\n");
        free(newPlayer);
        return;
    }

    newPlayer->role=role;

    printf("Enter Total Runs: ");
    scanf("%d",&newPlayer->totalRuns);

    printf("Enter Batting Average: ");
    scanf("%f",&newPlayer->battingAverage);

    printf("Enter Strike Rate: ");
    scanf("%f",&newPlayer->strikeRate);

    printf("Enter Wickets: ");
    scanf("%d",&newPlayer->wickets);

    printf("Enter Economy Rate: ");
    scanf("%f",&newPlayer->economyRate);

    if (newPlayer->totalRuns<0 || newPlayer->battingAverage<0 ||
        newPlayer->strikeRate<0 || newPlayer->wickets<0 ||
        newPlayer->economyRate<0)
    {
        printf("Invalid player statistics!\n");
        free(newPlayer);
        return;
    }

    newPlayer->performanceIndex=
    calculateIndex(role,
                   newPlayer->battingAverage,
                   newPlayer->strikeRate,
                   newPlayer->wickets,
                   newPlayer->economyRate);

    newPlayer->nextTeam=NULL;
    newPlayer->nextRole=NULL;

    insertPlayer(index,newPlayer);

    printf("Player added successfully to Team %s!\n",teamsData[index].name);
}


// Free all player nodes
void freeMemory()
{
    for (int i=0;i<TEAM_COUNT;i++)
    {
        struct PlayerNode *temp=teamsData[i].head;

        while (temp!=NULL)
        {
            struct PlayerNode *next=temp->nextTeam;

            free(temp);

            temp=next;
        }

        teamsData[i].head=NULL;
        teamsData[i].tail=NULL;
    }
}


// Main function
int main()
{
    int choice;

    initializeData();

    printf("============================================================\n");
    printf("       ICC ODI Player Performance Analyzer\n");
    printf("============================================================\n");

    while (1)
    {
        printf("\n1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of Specific Role Across All Teams\n");
        printf("6. Exit\n");

        printf("============================================================\n");

        printf("Enter your choice: ");
        scanf("%d",&choice);

        if (choice==1)
        {
            addPlayer();
        }
        else if (choice==2)
        {
            displayTeam();
        }
        else if (choice==3)
        {
            displayTeams();
        }
        else if (choice==4)
        {
            displayTopK();
        }
        else if (choice==5)
        {
            displayAllRole();
        }
        else if (choice==6)
        {
            freeMemory();

            printf("Exiting program...\n");
            return 0;
        }
        else
        {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
