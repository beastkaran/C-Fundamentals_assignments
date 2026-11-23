#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

typedef struct{
    int id;
    char name[50];
    int age;
}User;

void createUser(){
    User u;
    printf("Enter ID: ");
    scanf("%d", &u.id);
    getchar();
    printf("Enter Name: ");
    fgets(u.name, sizeof(u.name), stdin);
    u.name[strcspn(u.name, "\n")] = '\0';
    printf("Enter Age: ");
    scanf("%d", &u.age);

    FILE *fp = fopen(FILENAME, "a");
    if (fp == NULL){
        printf("Error: could not open file.\n");
        return;
    }
    fprintf(fp, "%d|%s|%d\n",u.id,u.name,u.age);
    fclose(fp);

    printf("User added.\n");
}

void readUsers(){
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL){
        printf("Error: could not open file.\n");
        return;
    }
    User u;

    printf("\nID    Name            Age\n");
    while (fscanf(fp, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) == 3) {
        printf("%-5d %-15s %d\n", u.id, u.name, u.age);
    }
    fclose(fp);
}

void updateUser(){
    int targetId;
    printf("Enter ID to update: ");
    scanf("%d", &targetId);
    getchar();

    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL){
        printf("Error: could not open file.\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL){
        printf("Error: could not open temp file.\n");
        fclose(fp);
        return;
    }
    User u;

    while (fscanf(fp, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) == 3){
        if (u.id == targetId){
            printf("Enter new Name: ");
            fgets(u.name, sizeof(u.name), stdin);
            u.name[strcspn(u.name, "\n")] = '\0';
            printf("Enter new Age: ");
            scanf("%d", &u.age);
            getchar();
        }
        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);
    if (remove(FILENAME) != 0){
        printf("Error: could not remove old file.\n");
        return;
    }
    if (rename("temp.txt", FILENAME) != 0){
        printf("Error: could not rename temp file.\n");
        return;
    }

    printf("User updated.\n");
}

void deleteUser(){
    int targetId;
    printf("Enter ID to delete: ");
    scanf("%d", &targetId);
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL){
        printf("Error: could not open file.\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL){
        printf("Error: could not open temp file.\n");
        fclose(fp);
        return;
    }
    User u;

    while (fscanf(fp, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) == 3){
        if (u.id != targetId) {
            fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
        }
    }
    fclose(fp);
    fclose(temp);
    if (remove(FILENAME) != 0){
        printf("Error: could not remove old file.\n");
        return;
    }
    if (rename("temp.txt", FILENAME) != 0){
        printf("Error: could not rename temp file.\n");
        return;
    }
    printf("User deleted.\n");
}

int main(){
    int choice;
    while (1) {
        printf("\n1. Create \n2. Read \n3. Update \n4 Delete \n5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        if (choice == 1) createUser();
        else if (choice == 2) readUsers();
        else if (choice == 3) updateUser();
        else if (choice == 4) deleteUser();
        else if (choice == 5) break;
    }
    return 0;
}