#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define M 7
//#define N 5
//#define P 3

//Algo 1: Matrix decomposition
// Best running on processes >= M
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

void singleMultiply(int m, int n, int p, int mymat[m], int vec_dup[n][p], int myout[m][p]){
    int i=0, j=0;
    for(i=0; i<m; i++){
        for(j=0; j<p; j++){
            myout[i][j] = mymat[i]*vec_dup[i][j];
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

main(int argc, char *argv[]){
    int myrank, npes;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    int m = M, p = P, n = N;
    int temp_val;
    int mat[M][N], vec[N][P], vec_dup[N][P];
    int out[M][P];
    int mymat[N];
    int myout[N][P];
    int mysum[P]={0};
    double start = MPI_Wtime();
    if (myrank==0){
        fillSparseMatrix(m, n, mat);
    }

// setting datatypes for MPI processes
    MPI_Datatype col, coltype, row, rowtype;
    MPI_Type_vector(n, 1, n, MPI_INT, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1*sizeof(int), &coltype);
    MPI_Type_commit(&coltype);
// Scattering A to all processes
    MPI_Scatter(mat, n, coltype,
      mymat, n, coltype,
      0, MPI_COMM_WORLD);
//
    fillVector(n, p, vec);
    singleMultiply(m, n, p, mymat, vec, myout);

// //Summing values of each column in intermediate result
    int i=0, j=0;
    for(i=0; i<n; i++){
        mysum[j] = 0;
        for(j=0; j<p; j++){
            temp_val = myout[i][j];
            mysum[j]= mysum[j]+temp_val;
    }
}
// Gathering all values
    MPI_Gather(mysum, p, MPI_INT, out, p, MPI_INT, 0, MPI_COMM_WORLD);
    if (myrank==0){
        printf("Algorithm 1: Matrix decomposition \n \n");
        printf("Input matrix \n");
        printMatrix(m, n, mat);
        printf("Input vector \n");
        printMatrix(n, p, vec);
        printf("Final output \n");
        printMatrix(m, p, out);
        double finish = MPI_Wtime();
        printf("Finished in %f seconds. \n", finish - start);
    }
MPI_Finalize();
}

