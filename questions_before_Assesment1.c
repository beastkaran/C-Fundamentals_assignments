//------------STRING_MANIPULATION-------------
//Question asked by rahul sir on friday
#include <stdio.h>
#include <string.h>

int count_vowels_fun(char s[100]){
    int count_vowels=0;
    for (int i=0;s[i]!='\0';i++){
        if (s[i]=='a' || s[i]=='e' || s[i]=='i' || s[i]=='o' || s[i]=='u'){
            count_vowels++;
            }
    }
    return count_vowels;
}

int count_special_fun(char s[100]){
    int count_special=0;
    for (int i=0;s[i]!='\0';i++){
        if (s[i]=='@' || s[i]=='-' || s[i]=='&' || s[i]=='*' || s[i]=='!'){
            count_special++;
            }
    }
    return count_special;
}

int main(){
    char s[100];
    fgets(s,sizeof(s),stdin);
    int count_of_vowels;
    int count_of_special;

    count_of_vowels=count_vowels_fun(s);
    printf("Number of vowels=%d\n",count_of_vowels);
    count_of_special=count_special_fun(s);
    printf("Number of special characters=%d\n",count_of_special);

    return 0;
}

//--------2D_CRUD-----------------------

#include <stdio.h>
#include <stdlib.h>


void swap_rows(int **m,int rw1,int rw2,int *col){
    for (int j=0;j<*col;j++){
        swap(m,rw1,j,rw2,j);
    }
}
void swap(int **mt,int r1,int c1,int r2, int c2){
    int temp=mt[r1][c1];
    mt[r1][c1]=mt[r2][c2];
    mt[r2][c2]=temp;
}
void delete(int **mt,int r,int c){
    mt[r][c]=-1;
}

void display(int **mt,int r, int c){
    for (int i=0;i<r;i++){
        for (int j=0;j<c;j++){
            printf("%d ",mt[i][j]);
        }
        printf("\n");
    }
}

void update(int **mt,int r, int c, int upd){
    mt[r][c]=upd;
}

int main(){
    int rows;
    printf("Enter no. of rows=");
    scanf("%d",&rows);
    int columns;
    printf("Enter no. of columns=");
    scanf("%d",&columns);

    if (rows <= 0 || columns <= 0){
        printf("Invalid dimensions\n");
        return 1;
    }

    int **mat=malloc(rows*sizeof(int *));
    if (mat==NULL){
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i=0;i<rows;i++){
        mat[i]=malloc(columns*sizeof(int));

        if (mat[i]==NULL){
            printf("Memo allocation failed\n");
            for (int j=0;j<i;j++){
                free(mat[j]);
            }
            free(mat);
            return 1;
        }

        for (int j=0;j<columns;j++){
            mat[i][j]=-1;
        }
    }
    int choice=-1;
    int row;
    int col;

    while (choice){
        printf("1.insert 2.display 3.delete 4.update 5.swap 6.swap rows 7.exit. Enter your choice=");
        scanf("%d",&choice);

        if (choice==1){
            printf("Enter row no.=");
            scanf("%d",&row);
            printf("Enter col no.=");
            scanf("%d",&col);

            if (row<0 || row>=rows || col<0 ||col>=columns){
                printf("Invalid row or col");
                continue;
            }
            printf("Enter value to add=");
            scanf("%d",&mat[row][col]);
        }
        else if (choice==2){
            display(mat,rows,columns);
        }

        else if (choice==3){
            int pos_r;
            int pos_c;
            printf("Enter row pos of element to delete=");
            scanf("%d",&pos_r);
            
            printf("Enter col pos of element to delete=");
            scanf("%d",&pos_c);

            if (pos_r<0 || pos_r>=rows || pos_c<0 || pos_c>=columns){
                printf("Out of bounds");
                continue;
            }
            delete(mat,pos_r,pos_c);
        }

        else if (choice==4){
            int upd;
            int pos_r;
            int pos_c;
            printf("Enter row pos of element to delete=");
            scanf("%d",&pos_r);
            
            printf("Enter col pos of element to delete=");
            scanf("%d",&pos_c);

            printf("Enter updated element=");
            scanf("%d",&upd);

            if (pos_r<0 || pos_r>=rows || pos_c<0 || pos_c>=columns){
                printf("Out of bounds");
                continue;
            }
            update(mat,pos_r,pos_c,upd);
            
        }
        else if(choice==5){
            int row_1;
            int col_1;
            printf("Enter element 1 row=");
            scanf("%d",&row_1);
            printf("Enter element 1 col=");
            scanf("%d",&col_1);

            int row_2;
            int col_2;
            printf("Enter element 2 row");
            scanf("%d",&row_2);
            printf("Enter element 2 col=");
            scanf("%d",&col_2);

            if (row_1 < 0 || row_1 >= rows ||
                col_1 < 0 || col_1 >= columns ||
                row_2 < 0 || row_2 >= rows ||
                col_2 < 0 || col_2 >= columns){

                    printf("Invalid row or column\n");
                    continue;
                }
            
            swap(mat,row_1,col_1,row_2,col_2);
        }

        else if(choice==6){
            int row1;
            int row2;
            printf("Enter row 1=");
            scanf("%d",&row1);
            printf("Enter row 2=");
            scanf("%d",&row2);


            if(row1<0||row1>=rows ||row2<0||row2>=rows){
                printf("Invalid rows asked to swap\n");
                continue;
            }
            swap_rows(mat,row1,row2,&columns);
        }

        
        else if (choice==7){
            break;
        }
        else{
            printf("Invalid choice");
        }
    }

    for (int i=0;i<rows;i++){
            free(mat[i]);
    }
    free(mat);

    return 0;
}


//---------------STRING---------------------
//string reverse using pointers
#include <stdio.h>
#include <string.h>

void reverse(char *s) {
    int len=strlen(s);
    int l=0,r=len-1;

    while (l<r){
        char temp=*(s+l);
        *(s+l)=*(s+r);
        *(s+r)=temp;
        l++;
        r--;
    }
}

int main() {
    char s[100];
    fgets(s,sizeof(s),stdin);

    s[strcspn(s,"\n")]='\0';
    reverse(s);

    printf("string reversed= = %99s\n", s);

    return 0;
}

//---------------------STRUCT---------------------------
#include <stdio.h>
#include <string.h>

struct Employee{
    int id;
    char name[50];
    float salary;
};

void update_struct(struct Employee *e1,char full_name[100]){
    strncpy(e1->name,full_name,sizeof(e1->name)-1);
    e1->name[sizeof(e1->name)-1]='\0';
}


void display_struct(struct Employee *e1){
    printf("ID:%d\n",e1->id);
    printf("Name:%s\n",e1->name);
    printf("Salary:%.2f\n",e1->salary);
}

int main(){
    struct Employee emp1;

    printf("Enter employee id=");
    scanf("%d",&emp1.id);
    printf("Enter employee name=");
    scanf("%49s",emp1.name);
    printf("Enter employee salary=");
    scanf("%f",&emp1.salary);

    int ch;
    while ((ch=getchar())!='\n' && ch!=EOF){
    }

    char full_name[100];
    printf("Enter full name for update=");
    fgets(full_name,sizeof(full_name),stdin);
    full_name[strcspn(full_name,"\n")]='\0';

    update_struct(&emp1,full_name);
    display_struct(&emp1);

    return 0;
}