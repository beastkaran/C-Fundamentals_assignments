#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//each cell of the matrix is a pointer to an int, so rotation = swapping pointers
void swap_ptr(int **a, int **b){
    int *temp=*a;
    *a=*b;
    *b=temp;
}

void print_matrix(int **mat, int n){
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            printf("%4d",**(mat+i*n+j));
        }
        printf("\n");
    }
}

void transpose(int **mat, int n){
    for (int i=0;i<n;i++){
        for (int j=i+1;j<n;j++){
            swap_ptr(mat+i*n+j, mat+j*n+i);
        }
    }
}

void reverse_rows(int **mat, int n){
    for (int i=0;i<n;i++){
        int l=0,r=n-1;
        while (l<r){
            swap_ptr(mat+i*n+l, mat+i*n+r);
            l++;
            r--;
        }
    }
}

//rotate 90 clockwise = transpose + reverse every row
void rotate(int **mat, int n){
    transpose(mat,n);
    reverse_rows(mat,n);
}

//prev and curr keep the ORIGINAL values of row i-1 and row i (O(N) extra space)
//row i+1 is not touched yet, so it is still original
void smooth(int **mat, int n){
    int *prev=malloc(n*sizeof(int));
    int *curr=malloc(n*sizeof(int));

    if (prev==NULL || curr==NULL){
        printf("Memo allocation failed.\n");
        return;
    }

    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            *(curr+j)=**(mat+i*n+j);
        }

        for (int j=0;j<n;j++){
            int sum=0;
            int count=0;

            for (int di=-1;di<=1;di++){
                for (int dj=-1;dj<=1;dj++){
                    int ni=i+di;
                    int nj=j+dj;

                    if (ni<0 || ni>=n || nj<0 || nj>=n){
                        continue;
                    }

                    if (di==-1){
                        sum+=*(prev+nj);
                    }
                    else if (di==0){
                        sum+=*(curr+nj);
                    }
                    else{
                        sum+=**(mat+ni*n+nj);
                    }
                    count++;
                }
            }
            **(mat+i*n+j)=sum/count;
        }

        int *temp=prev;
        prev=curr;
        curr=temp;
    }
    free(prev);
    free(curr);
}

int main(){
    int n;
    printf("Enter matrix size (2-10): ");
    scanf("%d",&n);
    if (n<2 || n>10){
        printf("Enter value in range of (2-10)\n");
        return 0;
    }

    int **mat=malloc(n*n*sizeof(int *));
    if (mat==NULL){
        printf("Memo allocation failed.\n");
        return 1;
    }

    srand(time(NULL));
    for (int i=0;i<n*n;i++){
        *(mat+i)=malloc(sizeof(int));
        **(mat+i)=rand()%256;
    }

    printf("Original Randomly Generated Matrix:\n");
    print_matrix(mat,n);

    rotate(mat,n);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    print_matrix(mat,n);

    smooth(mat,n);
    printf("\nMatrix after Applying 3×3 Smoothing Filter:\n");
    print_matrix(mat,n);

    for (int i=0;i<n*n;i++){
        free(*(mat+i));
    }
    free(mat);
    return 0;
}