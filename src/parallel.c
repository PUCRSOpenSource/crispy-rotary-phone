#include <mpi.h>
#include <omp.h>
#include <stdio.h>

#define ROWS 1000
#define COLUMNS 100000
#define MIN(a,b) (((a)<(b))?(a):(b))

int matrix[ROWS][COLUMNS];

void populate_matrix() {
	int i,j;
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			matrix[i][j] = COLUMNS - j;
		}
	}
}

int compare(const void *a, const void *b) {
	return (*(int *) a - *(int *) b);
}

int main(int argc, char *argv[]){
	int my_rank;
	int proc_n;
	int omp_rank;
	MPI_Status status;
	int* x;
	int y = ROWS/omp_get_thread_num();;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
	if (my_rank == 0) {
		populate_matrix();
	}
	else{
            MPI_Recv(x, COLUMNS*y,
                     MPI_INT, 0, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);
		#pragma omp parallel private(omp_rank)
		{
			omp_rank=omp_get_thread_num();
			printf("%d %d \n",my_rank,omp_rank);
		}
	}
	MPI_Finalize();
}
