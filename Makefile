all: sense_reversing_barrier tournament_barrier_mp dissemination_barrier tournament_barrier_mpi combined_barrier 

sense_reversing_barrier: 
		gcc -g -o sense_reversing_barrier sense_reversing_barrier.c -lm -fopenmp

tournament_barrier_mp: 
		gcc -g -o tournament_barrier_mp tournament_barrier_mp.c -lm -fopenmp
		
dissemination_barrier:
		mpicc dissemination_barrier.c -o dissemination_barrier -lm
		
tournament_barrier_mpi:
		mpicc tournament_barrier_mpi.c -o tournament_barrier_mpi -lm
		
combined_barrier:
		mpicc.mpich2 combined_barrier.c -o combined_barrier -lm  -fopenmp 
		
		
		
		
