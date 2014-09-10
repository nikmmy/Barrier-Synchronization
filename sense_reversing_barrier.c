#include <omp.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

#define NUM_BARRIERS 1000

double time[10];

sense_reversing_barrier(int *count, int *sense, int total){
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
  int thread_num,pub, priv, sense, total, sense1, count, i,k=0, time_spent;
  double start, stop;
  int NUM_THREADS = atoi(argv[1]);
  //Looping for 1000 barriers
  
  for(i=0; i<NUM_BARRIERS;i++)
  {
	  printf("\n***BARRIER %d***\n", i);
	  sense =0;
	  total = -1;
	  pub = 0;
	  thread_num = -1;
	#pragma omp parallel num_threads(NUM_THREADS) firstprivate(thread_num, priv, start, stop) shared(pub, count, sense)
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
		
		
		printf("\nthread %d: hello", thread_num);

		#pragma omp critical
			{
			  
			  pub += thread_num;
			  
			}
		
		//Each thread does some work
		k=0;
		while(k<10000000)
			k++;
		
		start = omp_get_wtime();
		sense_reversing_barrier(&count, &sense, total);
		stop = omp_get_wtime();
		
	
		printf("\nthread %d: final value of pub = %d", thread_num, pub);
		
		/*Adding to global time*/
		time[thread_num]  += (stop -start);
	
	  }
	 
  }
  for(k=0; k<8;k++)
   {
	   if(time[k]>0)
			printf("\nTime taken by thread no. %d across barriers: %lf", k, time[k]);
  }
  return 0;
}
