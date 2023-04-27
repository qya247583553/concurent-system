#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_PARTIES 3

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int partial_vote_counts[NUM_PARTIES] = {0};

    // 计算此投票中心的部分结果
    if (rank == 1) {
        partial_vote_counts[0] = 100;
        partial_vote_counts[1] = 200;
        partial_vote_counts[2] = 300;
    } else if (rank == 2) {
        partial_vote_counts[0] = 400;
        partial_vote_counts[1] = 500;
        partial_vote_counts[2] = 600;
    } else if (rank == 3) {
        partial_vote_counts[0] = 700;
        partial_vote_counts[1] = 800;
        partial_vote_counts[2] = 900;
    }

    // 将部分结果发送到主节点
    int master_rank = 0;
    MPI_Send(partial_vote_counts, NUM_PARTIES, MPI_INT, master_rank, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
