#include <mpi.h>
#include <omp.h>
#include <stdio.h>

#define COLUMNS 10
#define ROWS 8
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

int main(int argc, char *argv[]){
	int my_rank;
	int proc_n;
	int omp_rank;
	MPI_Status status;
	int* x;
	int i;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

	if (my_rank == 0){
		int work_sent = ROWS;
		int work_received = ROWS;
		int work_size = ((proc_n - 1)*COLUMNS)/ROWS;
		populate_matrix();

		while(work_received > 0) {
			for (i = 1; i < proc_n && work_sent > 0; ++i, work_sent-=2) {
				MPI_Send(matrix[work_sent], COLUMNS*(proc_n-1),
						MPI_INT, i, WORK_TAG,
						MPI_COMM_WORLD);
			}
			for (i = 1; i < proc_n && work_received > 0; ++i, work_received-=2) {
				MPI_Recv(matrix[work_received], COLUMNS*(proc_n-1),
						MPI_INT, i, WORK_TAG,
						MPI_COMM_WORLD, &status);
				printf("Process number: %d -> ", i);
				print_array(matrix[i]);
				printf("Process number: %d -> ", i);
				print_array(matrix[i+1]);
				printf("\n");
			}
		}

		int terminator = proc_n;
		while (--terminator) {
			MPI_Send(0, 0, MPI_INT, terminator,
					SUICIDE_TAG, MPI_COMM_WORLD);
			
		}
	}
	else{
		int work_pool[(proc_n-1)][COLUMNS];
		MPI_Recv(work_pool, COLUMNS*(proc_n-1),
				MPI_INT, 0, MPI_ANY_TAG,
				MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == SUICIDE_TAG) {
			MPI_Finalize();
			return 0;
		}
		#pragma omp parallel shared(work_pool, my_rank) private(omp_rank)
		{
			omp_rank=omp_get_thread_num();
			qsort(work_pool[omp_rank], COLUMNS, sizeof(int), compare);
			printf("my_rank: %d omp_rank: %d\n", my_rank, omp_rank);
			print_array(work_pool[omp_rank]);
		}
		#pragma omp barrier
		MPI_Send(work_pool, COLUMNS,
				MPI_INT, 0, WORK_TAG,
				MPI_COMM_WORLD);
	}
	MPI_Finalize();
}
