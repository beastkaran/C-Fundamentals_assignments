#include <stdio.h>

struct Student {
    int roll;
    char name[100];
    int marks[3];
};

int total_fun(int marks[3]){
    int total;
    total= marks[0]+marks[1]+marks[2];
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

int readMark(){
    int m;
    scanf("%d", &m);
    while (m < 0 || m > 100){
        printf("Invalid mark. Enter a value between 0 and 100: ");
        scanf("%d", &m);
    }
    return m;
}

void printRollsRecursive(struct Student students[], int remaining) {
    if (remaining==0){
        return;
    }
    printf("%d", students->roll);
    if (remaining>1) {
        printf(" ");
    }
    printRollsRecursive(students+1, remaining-1);
}

int main() {
    int N;
    struct Student students[100];
    scanf("%d", &N);
    int i;
    for (i = 0; i<N; i++) {
        scanf("%d",&students[i].roll);
        scanf("%s",students[i].name);
        
        students[i].marks[0]=readMark();
        students[i].marks[1]=readMark();
        students[i].marks[2]=readMark();
    }
    for (i=0; i<N; i++) {
        int total=total_fun(students[i].marks);
        float avg=average_fun(students[i].marks);
        char g=grade_fun(avg);

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
        if (g=='A') stars=5;
        else if (g=='B')stars=4;
        else if (g=='C')stars=3;
        else if (g=='D')stars=2;

        printf("Performance: ");
        int j;
        for (j=0; j<stars; j++) {
            printf("*");
        }
        printf("\n\n");
    }
    printf("List of Roll Numbers (via recursion): ");
    printRollsRecursive(students, N);
    printf("\n");
    return 0;
}
