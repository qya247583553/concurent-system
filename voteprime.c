#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_PARTIES 3

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total_vote_counts[NUM_PARTIES] = {0};
    int partial_vote_counts[NUM_PARTIES];

    // 接收每个投票中心的部分结果
    for (int i = 1; i < size; i++) {
        MPI_Recv(partial_vote_counts, NUM_PARTIES, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < NUM_PARTIES; j++) {
            total_vote_counts[j] += partial_vote_counts[j];
        }
    }

    // 输出总体选举结果
    printf("总体选举结果：\n");
    for (int i = 0; i < NUM_PARTIES; i++) {
        printf("政党%d: %d 票\n", i, total_vote_counts[i]);
    }

    MPI_Finalize();
    return 0;
}
