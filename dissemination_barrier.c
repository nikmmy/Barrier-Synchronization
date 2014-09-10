#include<stdio.h>
#include<math.h>
#include <mpi.h>
#include<sys/time.h>
#define NUM_BARRIERS 1000

/*The barrier function*/
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

int main(int argc, char** argv){
int numprocs, rank,i,k=0;

double time=0,t1,t0;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Barrier(MPI_COMM_WORLD);

for(i=0;i<NUM_BARRIERS;i++)
{
printf("\nBarrier %d: Rank of current processor is %d",i, rank);

//Each processor does some work
 k=0;
		while(k<10000000)
			k++;
			
//Computing time taken by barrier
t0=MPI_Wtime();
dissemination_barrier(numprocs, rank);
t1 = MPI_Wtime();
time+=t1-t0;
}

printf("\nTime taken by processor %d to complete is: %f\n",rank,time/NUM_BARRIERS);
MPI_Finalize();
return 0;
}
