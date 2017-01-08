#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#define NUMBER_TAG  1
#define FACTORS_TAG 2

/*
  This is a simple function that can be used to determine the number
  of factors for a given integer i.
*/
int getFactorCount(const long long n) {
  long long i = 0;
  int factorCount = 0;
  
  for(i = 1; (i <= n); i++) {
    if (!(n % i)) {
      factorCount++;
    }
  }
  return factorCount;
}

void doWorkerTasks() {
  long long number;
  do {
    /* read number from the manager (rank == 0) */
		MPI_Recv(&number, 1, MPI_LONG_LONG_INT, 0, NUMBER_TAG,
			 MPI_COMM_WORLD, NULL);
	  if (number != -1) {
      int factors = getFactorCount(number);
	  /* send number of factors back to the manager */
	  MPI_Send(&factors, 1, MPI_INT, 0, FACTORS_TAG, MPI_COMM_WORLD);
    }
  } while (number != -1);
}

void doManagerTasks(int size) {
	
  //int i = 1;
  //int status = 1;
	int factors = 0;
	MPI_Status receive_status;
	std::ifstream data("numbers_100.txt");
	if(!data.good()){
		std::cout << "unable to open the input file" << std::endl;
	}
	else {
		long long int number[size-1];
		while(!data.eof()) {	 
			
			for(int j = 1; j < size; j++) {
				data >> number[j-1];
		  /* Send number to be converted to the worker */
				MPI_Send(&number[j-1], 1, MPI_LONG_LONG_INT, j, NUMBER_TAG, MPI_COMM_WORLD);
			}
			for(int j = 1; j < size; j++) {
				MPI_Recv(&factors, 1, MPI_INT, j, FACTORS_TAG, MPI_COMM_WORLD, &receive_status);
				if(factors == 2) {
					printf("Number %lld has %d factors is prime.\n", number[j-1], factors);
				}
				else {
					printf("Number %lld has %d factors is not prime.\n",number[j-1], factors);
				}
			}
		}
	}
  /* send -1 to worker to inform worker to end processing */
  long long int number = -1;
  for(int j = 1; j < size; j++) {
	MPI_Send(&number, 1, MPI_LONG_LONG_INT, j, NUMBER_TAG, MPI_COMM_WORLD);
  }
}



int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size < 2) {
    printf("This program must be run with at least 2 processes!\n");
  } else {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
      doManagerTasks(size);
    } else {
      doWorkerTasks();
    }
  }
  
  MPI_Finalize(); 
  return 0;
}

// End of source code.
