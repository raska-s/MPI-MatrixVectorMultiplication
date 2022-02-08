#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Algo 2: Vector decomposition
//Best running on processes >=P
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

void transposeMatrix(int x, int y, int mat[x][y], int transpose [y][x]){
    int i=0, j=0;
    for (i = 0; i < x; i++)
        for (j = 0; j < y; j++) {
            transpose[j][i] = mat[i][j];
        }
}

void singleMultiply(int m, int n, int mymat[n], int vec[m][n], int myout[m][n]){
    int i=0, j=0;
    for(i=0; i<m; i++){
        for(j=0; j<n; j++){
            myout[i][j] = mymat[i]*vec[i][j];
        }
    }
}

void singleRowWiseAddition(int n, int p, int myout[n][p], int mysum[n]){
    int i=0, j=0;
    for(i=0; i<n; i++){
        mysum[j] = 0;
        for(j=0; j<p; j++){
            mysum[j]= mysum[j]+myout[i][j];
        }
    }
}

void singleColumnWiseAddition(int n, int p, int myout[n][p], int mysum[n]){
    int i=0, j=0;
    for(i=0; i<p; i++){
        mysum[j] = 0;
        for(j=0; j<n; j++){
            mysum[j]= mysum[j]+myout[i][j];
        }
    }
}


main(int argc, char *argv[]){
    int myrank, npes;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


    int m = M, p = P, n = N;
    int mat[M][N], vec[N][P], mat_T[N][M];
    int transpose[P][M];
    int out[M][P], out_T[P][M];
    int myvec[N];
    int myout[N][M], myout_T[M][N];
    int mysum[M]={0};
    double start = MPI_Wtime();

    if (myrank==0){
        fillVector(n, p, vec);
//Transpose to deal with non-contiguous data
        transposeMatrix(n, p, vec, transpose);
    }
//Setting datatypes for MPI processes
    MPI_Datatype col, coltype;
    MPI_Type_vector(n, 1, n, MPI_INT, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1*sizeof(int), &coltype);
    MPI_Type_commit(&coltype);
//Scattering B to all processes
    MPI_Scatter(transpose, n, coltype,
                myvec, n, coltype,
                0, MPI_COMM_WORLD);

    fillSparseMatrix(m, n, mat);
//Transpose to deal with non-contiguous data
    transposeMatrix(m, n, mat, mat_T);
//Performing elementwise multiplication
    singleMultiply(n, m, myvec, mat_T, myout);
//Summing values of each row in intermediate result
    int temp_val;
    int i=0, j=0;
    for(i=0; i<n; i++){
        mysum[j] = 0;
        for(j=0; j<m; j++){
            temp_val = myout[i][j];
            mysum[j]= mysum[j]+temp_val;
        }
    }
//Sending back to p0
    MPI_Gather(mysum, m, MPI_INT,
                out, m, MPI_INT,
                0, MPI_COMM_WORLD);
//Transpose to get the right output for printing
    transposeMatrix(p, m, out, out_T);

    if (myrank==0){
        printf("Algorithm 2: Vector decomposition \n \n");
        printf("Input matrix \n");
        printMatrix(m, n, mat);
        printf("Input vector \n");
        printMatrix(n, p, vec);
        printf("Final output \n");
        printMatrix(m, p, out_T);
        double finish = MPI_Wtime();
        printf("Finished in %f seconds. \n", finish - start);
    }
MPI_Finalize();
}

