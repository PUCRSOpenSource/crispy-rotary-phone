
## sequential.c
```c
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 100000
#define ROWS 10000
#define SUICIDE_TAG 2
#define WORK_TAG 1
#define THREADS 8

int matrix[ROWS][COLUMNS];
int run_quick = 1;

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

void bubble_sort(int size, int* array)
{
	int holder, swap = 1;
	for (int i = 0; i < size-1 && swap; ++i)
	{
		swap = 0;
		int limit = size - i -1;
		for (int j = 0 ; j < limit; ++j)
		{
			int current = array[j];
			int next = array[j + 1];
			if (current > next)
			{
				holder     = current;
				array[j]   = next;
				array[j+1] = holder;
				swap       = 1;
			}
		}
	}
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
	double t1, t2; 
	int my_rank;
	int proc_n;
	int omp_rank;
	int i;
	int work_sent = 0;
	int work_received = 0;
	run_quick = atoi(argv[1]);
	t1 = MPI_Wtime(); 
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

	if (my_rank == 0){
		populate_matrix();
		while(work_sent < ROWS) {
				for (i = 0; i < proc_n - 1 && work_sent < ROWS; ++i){
						MPI_Send(matrix[work_sent], THREADS*COLUMNS,
										MPI_INT, i+1, WORK_TAG,
										MPI_COMM_WORLD);
						work_sent+=THREADS;
				}
				for (i = 0; i < proc_n - 1 && work_received < ROWS; ++i) {
						MPI_Recv(matrix[work_received], THREADS*COLUMNS,
										MPI_INT, i+1, WORK_TAG,
										MPI_COMM_WORLD, &status);
						work_received+=THREADS;
				}
		}	
		int terminator = proc_n;
		while (--terminator)
			MPI_Send(0, 0, MPI_INT, terminator, SUICIDE_TAG, MPI_COMM_WORLD);

		print_matrix();
	}
	else{
		while(1) {
				int work_pool[THREADS][COLUMNS];
				MPI_Recv(work_pool, THREADS*COLUMNS,
								MPI_INT, 0, MPI_ANY_TAG,
								MPI_COMM_WORLD, &status);

				if (status.MPI_TAG == SUICIDE_TAG) {
						MPI_Finalize();
						return 0;
				}
				#pragma omp parallel for
				for (i = 0; i < THREADS; ++i) {
					if(run_quick){
						printf("quick\n");
						qsort(work_pool[i], COLUMNS, sizeof(int), compare);
					}
					else{
						printf("bubble\n");
						bubble_sort(COLUMNS, work_pool[i]);
					}
				}
				#pragma omp barrier
				MPI_Send(work_pool, THREADS*COLUMNS,
								MPI_INT, 0, WORK_TAG,
								MPI_COMM_WORLD);
		}
	}
	t2 = MPI_Wtime(); 
	MPI_Finalize();
	printf( "Elapsed time is %f\n", t2 - t1 ); 
}
```