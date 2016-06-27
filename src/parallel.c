#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 100
#define ROWS 4
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
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

	int work_size = ROWS * COLUMNS/(proc_n - 1);

	if (my_rank == 0){
		populate_matrix();
		for (i = 0; i < proc_n - 1; ++i){
			printf("work_size %d\n", work_size);
			printf("matrix(%d)+i(%d)*work_size(%d): %d\n", matrix, i, work_size, matrix+(i*work_size));
			MPI_Send(matrix+(i*work_size), work_size,
					MPI_INT, i+1, WORK_TAG,
					MPI_COMM_WORLD);
		}
		for (i = 0; i < proc_n - 1; ++i) {
			MPI_Recv(matrix+(i*work_size), work_size,
					MPI_INT, i+1, WORK_TAG,
					MPI_COMM_WORLD, &status);
		}

		int terminator = proc_n;
		while (--terminator) {
			MPI_Send(0, 0, MPI_INT, terminator,
					SUICIDE_TAG, MPI_COMM_WORLD);

		}
		print_matrix();
	}
	else{
		int work_pool[ROWS/(proc_n-1)][COLUMNS];
		MPI_Recv(work_pool, work_size,
				MPI_INT, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == SUICIDE_TAG) {
			MPI_Finalize();
			return 0;
		}
		printf("row to sort: %d\n", work_size/COLUMNS);
		#pragma omp parallel for
		for (i = 0; i < ROWS/proc_n; ++i) {
			qsort(work_pool[i], COLUMNS, sizeof(int), compare);
			printf("print array: \n");
			print_array(work_pool[i]);
		}
		#pragma omp barrier
		printf("send!!!\n");
		MPI_Send(work_pool, work_size,
				MPI_INT, 0, WORK_TAG,
				MPI_COMM_WORLD);
	}
	MPI_Finalize();
}
