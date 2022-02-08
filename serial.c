#include <stdio.h>
#include <time.h>

//#define M 7
//#define N 5
//#define P 3




void printMatrix(int x, int y, int mat[x][y]){
    int row, column=0;
    for (row=0; row<x; row++){
        for(column=0; column<y; column++){
            printf("%d     ", mat[row][column]);
        }
        printf("\n");
    }
    printf("\n");
}

void fillSparseMatrix(int x, int y, int matrix[x][y]){
    int i=0, j=0;
    for(i=0;i<x;i++){
        for(j=0;j<y;j++){
            if(i==j){
                matrix[i][j]=j+1;
            }
            else if(i*2==j){
                matrix[i][j]=j*2;
            }
            else if(i/2==j){
                matrix[i][j]=j*2;
            }
            else if(j*3==i){
                matrix[i][j]=j*3;
            }
            else if(j/3==i){
                matrix[i][j]=j*3;
            }

            else{
                matrix[i][j]=0;
            }
        }
    }
}

void fillVector(int x, int y, int vector[x][y]){
    int i=0, j=0;
    for(i=0;i<x;i++){
        for(j=0;j<y;j++){
            vector[i][j]=j+i+1;
//            printf("  %d", vector[i][j]);
        }
    }
}

void printVector(int len, int vector[len]){
    int i=0;
    for(i=0; i<len; i++){
         printf("%d    ", vector[i]);
    }
    printf("\n");
}


main(int argc, char *argv[]){
    int m=M, n=N, p=P;
    int mat[M][N];
    int vec[N][P];
    int out[M][P];
    int i, j, k, prod=0;
    int row, column=0;

    // measure elapsed wall time
    struct timespec now, tmstart;
    clock_gettime(CLOCK_REALTIME, &tmstart);

    fillSparseMatrix(m, n, mat);
    fillVector(n, p, vec);
//Printing inputs
    printf("SERIAL MATRIX MULTIPLICATION \n \n");

    printf("Input matrix \n");

    for (row=0; row<M; row++)
     {
        for(column=0; column<N; column++){
                printf("%d     ", mat[row][column]);
        }
        printf("\n");
     }
    printf("\n");

    printf("Input vector \n");
     for (row=0; row<N; row++)
     {
        for(column=0; column<P; column++){
            printf("%d     ", vec[row][column]);
        }
        printf("\n");
     }
    printf("\n");

//Performing for loop-based matrix multiplication
    for (i=0; i<M; i++){
        for (j=0; j<P; j++){
            for(k=0; k<N; k++){
                prod = prod + mat[i][k]*vec[k][j];
            }
            out[i][j] = prod;
            prod=0;
        }
    }
//Printing final output
    printf("Final output \n");
     for (row=0; row<M; row++){
        for(column=0; column<P; column++){
            printf("%d     ", out[row][column]);
        }
        printf("\n");
     }
//Getting time
    clock_gettime(CLOCK_REALTIME, &now);
    double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
    printf("Finished in %f seconds.\n\n", seconds);
}



