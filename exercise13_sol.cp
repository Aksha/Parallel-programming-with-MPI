#include <iostream>
#include <fstream>
#include <mpi.h>

const int NUMBER_TAG  = 1;
const int FACTORS_TAG = 2;

const int MANAGER_RANK = 0;
const int WORKER_RANK  = 1;

/**
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
    long long number[2];
    do {
        /* read number from the manager (rank == 0) */
        MPI_Recv(number, 2, MPI_LONG_LONG_INT, MANAGER_RANK, NUMBER_TAG,
                 MPI_COMM_WORLD, NULL);
        if (number[0] != -1) {
            int factors[2] = {getFactorCount(number[0]),
                              getFactorCount(number[1])};
            /* send number of factors back to the manager */
            MPI_Send(factors, 2, MPI_INT, MANAGER_RANK, FACTORS_TAG,
                     MPI_COMM_WORLD);
        }
    } while (number[0] != -1);
}

void doManagerTasks() {
    std::ifstream data("numbers.txt");
    if (!data.good()) {
        std::cout << "Unable to open numbers.txt for input. "
                  << "Manager aborting.\n";
    } else {
        while (!data.eof()) {
            long long number[2];
            int factors[2];
            data >> number[0] >> number[1];
            // Send number to be converted to the worker
            MPI_Send(number, 2, MPI_LONG_LONG_INT, WORKER_RANK, NUMBER_TAG,
                     MPI_COMM_WORLD);
            MPI_Recv(factors, 2, MPI_INT, WORKER_RANK, FACTORS_TAG,
                     MPI_COMM_WORLD, NULL);
            // Print results
            std::cout << "Number " << number[0] << " has " << factors[0]
                      << " factors.\n";
            std::cout << "Number " << number[1] << " has " << factors[1]
                      << " factors.\n";            
        }
    }
    // send -1 to worker to inform worker to end processing
    long long number[2] = {-1, -1};
    MPI_Send(&number, 2, MPI_LONG_LONG_INT, WORKER_RANK, NUMBER_TAG,
             MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size < 2) {
        std::cerr << "This program must be run with at least 2 processes!\n";
    } else {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0) {
            doManagerTasks();
        } else {
            doWorkerTasks();
        }
    }
    
    MPI_Finalize(); 
    return 0;
}

// End of source code.
