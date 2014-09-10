#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<mpi.h>
#include<sys/time.h>

#define winner 0
#define loser 1
#define bye 2
#define champion 3
#define dropout 4
#define NUM_BARRIERS 6
#define val 1

struct round_t
{
	int role;
	int opponent;
};

int size;

void tournament_barrier(struct round_t arr[size][10], int rank, int rounds)
{
	#if val == true
		printf("\nInside Tournament Barrier\n");	
	#endif
	int round=1,msg=1,tag1=1,tag2=1;
	MPI_Status stat;

	while(1)
	{
		#if val == true
			printf("\nRank %d inside first while loop",rank);
		#endif		
		if(arr[rank][round].role==loser)
		{
			int op_winner=arr[rank][round].opponent;
			MPI_Send(&msg,1,MPI_INT,op_winner,tag1,MPI_COMM_WORLD);
			break;
			
		}

		if(arr[rank][round].role==winner)
		{
			int op_loser=arr[rank][round].opponent;
			MPI_Recv(&msg,1,MPI_INT,op_loser,tag1,MPI_COMM_WORLD,&stat);
		}

		if(arr[rank][round].role==champion)
		{
			int op_runnerup=arr[rank][round].opponent;
			MPI_Recv(&msg,1,MPI_INT,op_runnerup,tag1,MPI_COMM_WORLD,&stat);
			break;
		}


		//if(round<rounds)
			round++;
	}

	#if val == true
		printf("\nRank %d Waking up...",rank);
	#endif
	//round=rounds;
	#if val == true
		printf("\nRank %d Round before second while loop: %d",rank,round);
	#endif
	while(1)
	{
		
		//printf("\nround in 2nd while loop: %d",round);
		#if val == true
			printf("\nRank %d inside second while loop",rank);
		#endif
		if(arr[rank][round].role == winner||arr[rank][round].role == champion) 
		{
			//printf("\nRank %d op_loser_to_waken: %d",rank,arr[rank][round].opponent);			
			int op_loser_to_waken = arr[rank][round].opponent;
			
			MPI_Ssend(&msg,1,MPI_INT,op_loser_to_waken,tag2,MPI_COMM_WORLD);
		}

		if(arr[rank][round].role == loser) 
		{
			//printf("\nRank %d op_loser_to_waken: %d",rank,arr[rank][round].opponent);			
			int op_winner1 = arr[rank][round].opponent;
			
			MPI_Recv(&msg,1,MPI_INT,op_winner1,tag2,MPI_COMM_WORLD,&stat);
		}

		if(arr[rank][round].role == dropout)
			break;

		//if(round > 0)
			round--;

	}

}

int main(int argc, char **argv)
{
	int rank;
	struct timeval stop,start;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int rounds=ceil(log(size)/log(2));

	//printf("\nrounds = %d",rounds);
	int i, j, k=0,val1,val2=0;
	j = rank;
	struct round_t arr[size][10];

	for( k=0; k<=rounds; k++ ) 
	{
		arr[j][k].role = -1;
		arr[j][k].opponent = -1;
	}

	for(k=0;k<=rounds;k++)
	{
		val1=ceil(pow(2,k));
		val2=ceil(pow(2,k-1));

		if(k==0)
			arr[j][k].role=dropout;	
		
		if(k>0 && j==0 && val1>=size)
			arr[j][k].role=champion;

		if(k>0 && (j%val1==val2))
			arr[j][k].role=loser;

		if(k>0 && (j%val1==0) && (j+val2)>=size)
			arr[j][k].role=bye;

		if(k>0 && (j%val1==0) && (j+val2)<size && val1<size)
			arr[j][k].role=winner;


		if(arr[j][k].role==loser)
			arr[j][k].opponent=j-val2;

		if(arr[j][k].role==winner || arr[j][k].role==champion)
			arr[j][k].opponent=j+val2;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double t0,t1,time=0.0;
	
	//gettimeofday(&start,0);
	
	for( i=0; i<NUM_BARRIERS; i++ )
	{
		//Make process do some work
		k=0;
		while(k<10000000)
			k++;
		printf("Proc %d waiting at barrier %d\n",rank, i);

		//Having completed work go to barrier
     	t0 = MPI_Wtime();
     	tournament_barrier(arr,rank,rounds);
     	t1 = MPI_Wtime();
       	time+=t1-t0;
	}
	
	printf("\nTime taken by proc %d is: %f\n",rank,time/NUM_BARRIERS);

	MPI_Finalize();
}
