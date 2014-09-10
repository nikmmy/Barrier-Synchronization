#include <omp.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>

# define NUM_BARRIERS 1000

double stop, start;

/*The OpenMPI barrier for synchronizing processes*/
void dissemination_barrier(int numprocs, int rank){
int i, to_send, msg =1, tag=1, to_recv;
MPI_Status stat;
int num_levels = ceil(log(numprocs)/log(2));

for(i=0; i<num_levels; i++){
	to_send = (rank + pow(2, i));
	to_send = to_send % numprocs;

	to_recv = (rank - pow(2, i) + numprocs);
	to_recv = to_recv %  numprocs;

	MPI_Send(&msg, 1, MPI_INT, to_send, tag, MPI_COMM_WORLD);
	MPI_Recv(&msg, 1, MPI_INT, to_recv, tag, MPI_COMM_WORLD, &stat);

 }
}

/*The centralized sense reversing barrier for synchronizing threads within a process*/
void sense_reversing_barrier(int *count, int *sense, int total){
	#pragma omp critical
		{
			*count = *count - 1;
	
			if(*count == 0)
			{
				*count = total;
				*sense = 1;
			}
		}	
	
	while(*sense == 0){};
}

int main(int argc, char **argv)
{
  
   
  /*Open MPI declarations*/
    int numprocs, rank, num_of_levels;
	
	
  /*Open MP declarations*/
  int NUM_THREADS,thread_num,pub, sense, total, count, i, k;
  NUM_THREADS = atoi(argv[1]);
  	  
	  /*Combined barrier starts with Open MPI part*/
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  start = omp_get_wtime();
  
  /*Looping for 1000 barriers*/
  for(i=0; i< NUM_BARRIERS; i++){
      
	   /*Open MP part starts*/
		  sense =0;
		  total = -1;
		  pub = 0;
		  thread_num = -1;
		  
		#pragma omp parallel num_threads(NUM_THREADS) firstprivate(thread_num, start, stop) shared(pub, count, sense)
		  {
			
			thread_num = omp_get_thread_num();
			
			
			#pragma omp critical
			 {
				if(total == -1)
					{
						count = omp_get_num_threads();
						total = omp_get_num_threads();
					}
			}
			while(total == -1);
			

			#pragma omp critical
				{
				  pub += thread_num;
				
				}
			
			sense_reversing_barrier(&count, &sense, total);
			
			printf("\nRound %d Rank: %d, Thread %d: Final value of pub = %d", i, rank, thread_num, pub);
			
		/*Open MP part ends*/
		}
	 }
			
		/*Open MPI part ends*/
		dissemination_barrier(numprocs, rank);
		stop = omp_get_wtime();
		printf("\nTime taken for processor %d to reach combined barrier is %lf", rank, stop  - start);
		
	
		MPI_Finalize();	
  return 0;
}

