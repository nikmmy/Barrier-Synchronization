/*Tournament Barrier implemented using OpenMP*/

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>

#define NOT_PARTICIPATING -1
#define CHAMP 555
#define LOSER 666

#define NUM_BARRIERS 1000

int threads[8][3];
int position[8][3];
int wakeup[8][3];
int done[8];
int sense = 0;
double time[10];

/*Initialization*/
void initialize(int total){
int i,j, num_of_levels, step;
num_of_levels = ceil(log(total)/log(2));


for(i=0;i<total;i++)
{
     done[i] = -1;
	{
		for(j=0; j< num_of_levels; j++)
			{
				position[i][j] = -1;
				threads[i][j] = -1;
				wakeup[i][j] = -1;
			}
	}
}
	


//Pre-decide who is the winner at each level
for(i=0; i<num_of_levels; i++){
   step  = pow(2,i);
   for(j=0; j< total && ((j+step)<total); j= j + (2*step)){
	   position[j][i] = CHAMP;
	   position[j+step][i] = j;
	   wakeup[j][i] = j+step;
	   wakeup[j+step][i] = LOSER;
	}     
}

}

/*Wake up*/
void signal_all(int total){
	int i,j, to_be_woken, num_of_levels, thread_num;
		done[0] = 1;
		num_of_levels = ceil(log(total)/log(2));
		for(i= (num_of_levels-1); i>=0; i--)
			for(j=0; j< total; j++)
			 {
				if(wakeup[j][i]!=LOSER && wakeup[j][i]!=NOT_PARTICIPATING){
					to_be_woken = wakeup[j][i];
					done[to_be_woken] = 1;
				}
			 }
	
}

/*Arrival tree*/
void tournament_barrier(int total, int thread_num){
	int i, winner_index, num_of_levels;
	double temp;
	temp = log(total)/log(2);
	num_of_levels = ceil(temp);
	
	for(i=0; i<num_of_levels; i++){
	
		//if it is a champ thread
		if(position[thread_num][i] == CHAMP){
			while(1){
				if(threads[thread_num][i] == 1)
					break;
			}
		}
		
		//else if it is a loser thread
		else if(position[thread_num][i]!= NOT_PARTICIPATING){
			 winner_index = position[thread_num][i];
			 threads[winner_index][i] = 1;
			 if((winner_index == 0) && (i == (num_of_levels - 1)))
				{
						printf("Yeah done");
						signal_all(total);
				}
		
		}
	}
 
 while(1){
		if(done[thread_num] == 1)
			break;
 }
}

int main(int argc, char **argv)
{
  int thread_num ,priv, pub, total,i, k, threads_count;
  double start, stop;
  
  threads_count = atoi(argv[1]);
  //For loop for num of barriers
  for(i=0; i< NUM_BARRIERS;i++)
  {
	  printf("\n***BARRIER***%d\n", i);
	  pub = 0; total = -1; thread_num = -1;
	  initialize(threads_count);
	  #pragma omp parallel num_threads(threads_count) firstprivate(thread_num, start, stop) shared(pub)
	  {
		
		thread_num = omp_get_thread_num();
		
		
		#pragma omp critical
		 {
			if(total == -1)
				{
					total = omp_get_num_threads();
				}
		}
		while(total == -1);
		
		
		printf("thread %d: hello\n", thread_num);

		#pragma omp critical
			{
			  pub += thread_num;
			}
	
	   //Each thread does some work		
		k=0;
		while(k<10000000)
			k++;
		
		//Calculating time
		start = omp_get_wtime();
		tournament_barrier(threads_count, thread_num);
		stop = omp_get_wtime();
		
		printf("\nthread %d: final value of pub = %d", thread_num, pub);
		time[thread_num] += (stop -start);
	  }
  }
for(k=0; k<8;k++)
   {
	   if(time[k]>0)
			printf("\nTime taken by thread no. %d across barriers: %lf", k, time[k]);
  }
  return 0;
}
