#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REPORTS 10

typedef struct Employee{
    int empID;
    char name[50];
    char designation[50];
    struct Employee *reportingManager;
    struct Employee *reports[MAX_REPORTS];
} Employee;

Employee *allEmployees[100];
int employeeCount=0;


Employee* findEmployeeByID(int empID){
    for(int i=0;i<employeeCount;i++){
        if(allEmployees[i]->empID==empID)
            return allEmployees[i];
    }
    return NULL;
}


int countReports(Employee *emp){
    int count=0;

    for(int i=0;i<MAX_REPORTS;i++){
        if(emp->reports[i]!=NULL)
            count++;
    }

    return count;
}


void addEmployee(){
    int id,managerID;

    if(employeeCount>=100){
        printf("Employee limit reached!\n");
        return;
    }

    printf("Enter Employee ID: ");
    scanf("%d",&id);

    if(findEmployeeByID(id)!=NULL){
        printf("Employee ID already exists!\n");
        return;
    }

    Employee *newEmp=malloc(sizeof(Employee));

    if(newEmp==NULL){
        printf("Memory allocation failed!\n");
        return;
    }

    newEmp->empID=id;
    newEmp->reportingManager=NULL;

    for(int i=0;i<MAX_REPORTS;i++)
        newEmp->reports[i]=NULL;

    printf("Enter Employee Name: ");
    scanf("%49s",newEmp->name);

    printf("Enter Employee Designation: ");
    scanf("%49s",newEmp->designation);

    printf("Enter Reporting Manager ID (-1 if none): ");
    scanf("%d",&managerID);

    if(managerID!=-1){
        Employee *manager=findEmployeeByID(managerID);

        if(manager==NULL){
            printf("Manager not found!\n");
            free(newEmp);
            return;
        }

        if(countReports(manager)>=MAX_REPORTS){
            printf("Manager has too many direct reports!\n");
            free(newEmp);
            return;
        }

        int i;
        for(i=0;i<MAX_REPORTS;i++){
            if(manager->reports[i]==NULL){
                manager->reports[i]=newEmp;
                break;
            }
        }

        newEmp->reportingManager=manager;
    }

    allEmployees[employeeCount]=newEmp;
    employeeCount++;

    printf("Employee added successfully!\n");
}


void removeEmployee(){
    int id;

    printf("Enter Employee ID to remove: ");
    scanf("%d",&id);

    Employee *emp=findEmployeeByID(id);

    if(emp==NULL){
        printf("Employee not found!\n");
        return;
    }

    if(strcmp(emp->designation,"CEO")==0){
        printf("CEO cannot be removed!\n");
        return;
    }

    Employee *manager=emp->reportingManager;
    Employee *sibling=NULL;

    int childCount=countReports(emp);
    int minReports=MAX_REPORTS+1;

    //Find the sibling with the fewest direct reports
    if(childCount>0 && manager!=NULL){
        for(int i=0;i<MAX_REPORTS;i++){
            Employee *temp=manager->reports[i];

            if(temp!=NULL && temp!=emp){
                int count=countReports(temp);

                if(count<minReports){
                    minReports=count;
                    sibling=temp;
                }
            }
        }
    }
    // If there are no siblings, use the superior
    if(childCount>0 && manager==NULL){
        printf("Cannot transfer reports. No superior exists!\n");
        return;
    }

    if(sibling==NULL && childCount>0){
        sibling=manager;
    }

    //Check whether the selected employee can accommodate all the direct reports.
    if(childCount>0 &&
       countReports(sibling)+childCount>MAX_REPORTS){
        printf("Not enough space to transfer reports!\n");
        return;
    }

    // Remove employee from the manager's report list.
    if(manager!=NULL){
        for(int i=0;i<MAX_REPORTS;i++){
            if(manager->reports[i]==emp){
                manager->reports[i]=NULL;
                break;
            }
        }
    }
    //Transfer direct reports to the selected sibling or superior.
    if(childCount>0){
        for(int i=0;i<MAX_REPORTS;i++){
            if(emp->reports[i]!=NULL){
                Employee *child=emp->reports[i];

                for(int j=0;j<MAX_REPORTS;j++){
                    if(sibling->reports[j]==NULL){
                        sibling->reports[j]=child;
                        child->reportingManager=sibling;
                        break;
                    }
                }

                emp->reports[i]=NULL;
            }
        }
    }

    //Remove employee from allEmployees.
    for(int i=0;i<employeeCount;i++){
        if(allEmployees[i]==emp){
            for(int j=i;j<employeeCount-1;j++)
                allEmployees[j]=allEmployees[j+1];

            employeeCount--;
            break;
        }
    }
    free(emp);
    printf("Employee removed successfully!\n");
}

void displayRecursive(Employee *emp,int level){
    if(emp==NULL)
        return;

    for(int i=0;i<level;i++)
        printf("    ");

    printf("%d - %s - %s\n", emp->empID,emp->name,emp->designation);

    for(int i=0;i<MAX_REPORTS;i++){
        if(emp->reports[i]!=NULL)
            displayRecursive(emp->reports[i],level+1);
    }
}

int countEmployeesUnder(Employee *emp){
    int count=0;

    for(int i=0;i<MAX_REPORTS;i++){
        if(emp->reports[i]!=NULL){
            count++;
            count+=countEmployeesUnder(emp->reports[i]);
        }
    }

    return count;
}

void displayHierarchy(){
    int id;

    printf("Enter Employee ID: ");
    scanf("%d",&id);

    Employee *emp=findEmployeeByID(id);

    if(emp==NULL){
        printf("Employee not found!\n");
        return;
    }

    printf("\n===== EMPLOYEE HIERARCHY =====\n");

    displayRecursive(emp,0);

    printf("Total Employees Under %d: %d\n",
           id,countEmployeesUnder(emp));
}

void displayCompleteHierarchy(){
    printf("\n===== ORGANIZATION HIERARCHY =====\n");

    for(int i=0;i<employeeCount;i++){
        if(allEmployees[i]->reportingManager==NULL)
            displayRecursive(allEmployees[i],0);
    }

    printf("Total Employees: %d\n",employeeCount);
}

int main(){
    int choice;

    while(1){
        printf("\n1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");
        scanf("%d",&choice);

        if(choice==1) addEmployee();
        else if(choice==2) removeEmployee();
        else if(choice==3) displayHierarchy();
        else if(choice==4) displayCompleteHierarchy();
        else if(choice==5){
            for(int i=0;i<employeeCount;i++)
                free(allEmployees[i]);

            return 0;
        }
        else
            printf("Invalid choice!\n");
    }
}