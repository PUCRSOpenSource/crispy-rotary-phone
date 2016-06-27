#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 100
#define ROWS 16
#define SUICIDE_TAG 2
#define WORK_TAG 1

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

void print_array(int array[]) {
	int i;
	for (i = 0; i < 10; ++i) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

void print_matrix()
{
	int i,j;
	for (i = 0; i < ROWS; ++i) {
		for (j = 0; j < 30; ++j) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]){
	int my_rank;
	int proc_n;
	int omp_rank;
	int i;
	int work = 0;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

	if (my_rank == 0){
		populate_matrix();
		for (i = 0; i < proc_n - 1 && work < ROWS; ++i){
			printf("i: %d\n", i);
			MPI_Send(matrix[work], 8*COLUMNS,
					MPI_INT, i+1, WORK_TAG,
					MPI_COMM_WORLD);
			work+=8;
		}
		for (i = 0; i < proc_n - 1 && work > 0; ++i) {
			MPI_Recv(matrix[work], 8*COLUMNS,
					MPI_INT, i+1, WORK_TAG,
					MPI_COMM_WORLD, &status);
			work+=8;
		}

		int terminator = proc_n;
		while (--terminator)
			MPI_Send(0, 0, MPI_INT, terminator, SUICIDE_TAG, MPI_COMM_WORLD);

		print_matrix();
	}
	else{
		int work_pool[8][COLUMNS];
		MPI_Recv(work_pool, 8*COLUMNS,
				MPI_INT, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == SUICIDE_TAG) {
			MPI_Finalize();
			return 0;
		}
		#pragma omp parallel for
		for (i = 0; i < 8; ++i) {
			qsort(work_pool[i], COLUMNS, sizeof(int), compare);
		}
		#pragma omp barrier
		printf("send!!!\n");
		MPI_Send(work_pool, 8*COLUMNS,
				MPI_INT, 0, WORK_TAG,
				MPI_COMM_WORLD);
	}
	MPI_Finalize();
}
