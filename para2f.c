#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
// Algo 3: Matrix-vector decomposition
//Best running on N processes

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
//            printf(" %d    ", mat[i][j]);
        }

}

void singleMultiply(int m, int p, int mymat[m], int myvec[p], int myout[m][p]){
    int i=0, j=0;
    for(i=0; i<m; i++){
        for(j=0; j<p; j++){
            myout[i][j] = mymat[i]*myvec[j];
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

    int mat[M][N], vec[N][P];
    int transpose[N][M];
    int out[M][P]={0};
    int myvec[P]={0};
    int mymat[M]={0};
    int myout[M][P]={0};
    double start = MPI_Wtime();

    if (myrank==0){
        fillSparseMatrix(m, n, mat);
        fillVector(n, p, vec);
        transposeMatrix(m, n, mat, transpose);

    }
//Setting datatypes for MPI processes
    MPI_Datatype col, coltype, row, rowtype;

    MPI_Type_vector(m, 1, m, MPI_INT, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1*sizeof(int), &coltype);
    MPI_Type_commit(&coltype);

    MPI_Type_vector(p, 1, p, MPI_INT, &row);
    MPI_Type_commit(&row);
    MPI_Type_create_resized(row, 0, 1*sizeof(int), &rowtype);
    MPI_Type_commit(&rowtype);
//Scatter A and B to all processes
    MPI_Scatter(transpose, m, coltype,
              mymat, m, coltype,
              0, MPI_COMM_WORLD);

    MPI_Scatter(vec, p, rowtype,
              myvec, p, rowtype,
              0, MPI_COMM_WORLD);
//Performing tensor product
    int i=0, j=0;
    for(i=0; i<m; i++){
        for(j=0; j<p; j++){
            myout[i][j] = mymat[i]*myvec[j];
        }
    }
//Sending back to p0
    MPI_Reduce(myout, out, m*p, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myrank==0){
        printf("Algo 3: Matrix-vector decomposition \n \n");
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

