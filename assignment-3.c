#include <stdio.h>

struct Student {
    int roll;
    char name[100];
    int marks[3];
};
int studentsProcessed = 0;

int total_fun(int marks[3]){
    int total;
    total = marks[0] + marks[1] + marks[2];
    return total;
}

float average_fun(int marks[3]){
    int sum;
    sum = total_fun(marks);
    float avg = sum / 3.0;
    return avg;
}
char grade_fun(float avg){
    if (avg >=85){
        return 'A';
    }
    else if(avg >=70){
        return 'B';
    }
    else if(avg >=50){
        return 'C';
    }
    else if(avg >=35){
        return 'D';
    }
    else{
        return 'F';
    }
}
void printRollsRecursive(struct Student students[], int i, int N) {
    if (i >= N){
        return;
    }
    printf("%d", students[i].roll);
    if (i < N-1) {
        printf(" ");
    }
    printRollsRecursive(students, i+1,N);
}

int main() {
    int N;
    struct Student students[100];
    scanf("%d", &N);
    int i;
    for (i = 0; i<N; i++) {
        scanf("%d",&students[i].roll);
        scanf("%s",students[i].name);
        scanf("%d %d %d", &students[i].marks[0], &students[i].marks[1], &students[i].marks[2]);
    }
    for (i=0; i<N; i++) {
        int total=total_fun(students[i].marks);
        float avg=average_fun(students[i].marks);
        char g=grade_fun(avg);
        studentsProcessed++;

        printf("Roll: %d\n",students[i].roll);
        printf("Name: %s\n",students[i].name);
        printf("Total: %d\n",total);
        printf("Average: %.2f\n",avg);
        printf("Grade: %c\n",g);
        if (avg<35){
            printf("\n");
            continue;
        }

        int stars;
        if (g == 'A') stars=5;
        else if (g == 'B')stars=4;
        else if (g == 'C')stars=3;
        else stars = 2;

        printf("Performance: ");
        int j;
        for (j = 0; j < stars; j++) {
            printf("*");
        }
        printf("\n\n");
    }
    printf("List of Roll Numbers (via recursion): ");
    printRollsRecursive(students, 0, N);
    printf("\n");
    return 0;
}