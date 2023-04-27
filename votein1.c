#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_PARTIES 4
#define NUM_REGIONS 3

int main(int argc, char** argv) {

    int rank, size, i, j;
    int party_votes[NUM_PARTIES] = {0}; // 初始化每个政党的得票数为0
    int region_votes[NUM_REGIONS][NUM_PARTIES] = {0}; // 初始化每个地区中心的投票结果为0

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NUM_REGIONS + 1) { // 检查节点数量是否正确
        printf("需要 %d 个节点，当前只有 %d 个节点\n", NUM_REGIONS + 1, size);
        MPI_Finalize();
        exit(1);
    }

    if (rank == 0) { // 主节点
        for (i = 1; i < size; i++) {
            MPI_Recv(&region_votes[i-1], NUM_PARTIES, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (j = 0; j < NUM_PARTIES; j++) {
                party_votes[j] += region_votes[i-1][j];
            }
        }

        // 输出每个政党的得票数
        printf("每个政党的得票数：\n");
        for (i = 0; i < NUM_PARTIES; i++) {
            printf("政党 %d: %d\n", i+1, party_votes[i]);
        }
    } else { // 地区中心节点
        // 模拟每个地区中心的投票结果
        for (i = 0; i < NUM_PARTIES; i++) {
            region_votes[rank-1][i] = rand() % 1000;
        }
        MPI_Send(&region_votes[rank-1], NUM_PARTIES, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
