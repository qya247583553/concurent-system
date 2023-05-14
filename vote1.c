#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_PARTIES 4
#define NUM_REGIONS 3

int main(int argc, char** argv) {

    int rank, size, i, j;
    int party_votes[NUM_PARTIES] = {0}; //nitialize the number of votes for each party to 0
    int region_votes[NUM_REGIONS][NUM_PARTIES] = {0}; // Initialize the vote result of each regional center to 0

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NUM_REGIONS + 1) { // Check that the number of nodes is correct
      printf("Area %d vote message error，area %d vote num is%d\n", i+1,j+1,region_votes[i][j]);
        MPI_Finalize();
        exit(1);
    }

    if (rank == 0) { // main
        for (i = 1; i < size; i++) {
            MPI_Recv(&region_votes[i-1], NUM_PARTIES, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (j = 0; j < NUM_PARTIES; j++) {
                party_votes[j] += region_votes[i-1][j];
            }
        }

        // Output the number of votes for each political party
        printf("Votes for each political party：\n");
        for (i = 0; i < NUM_PARTIES; i++) {
            printf("parties %d: %d\n", i+1, party_votes[i]);
        }
    } else { // Regional Center Node
        // Simulate voting results for each regional center
        for (i = 0; i < NUM_PARTIES; i++) {
            region_votes[rank-1][i] = rand() % 1000;
        }
        MPI_Send(&region_votes[rank-1], NUM_PARTIES, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
