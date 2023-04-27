#include <iostream>
#include <vector>
#include <mpi.h>

int main(int argc, char** argv) {
    int num_processes, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> votes_per_party = {0, 0, 0, 0}; // 初始化每个政党的票数

    // 每个节点计算其分配的子任务
    int num_votes = 1000; // 假设每个节点有1000张选票
    for (int i = 0; i < num_votes; i++) {
        int party = rand() % 4; // 假设有4个政党
        votes_per_party[party]++;
    }

    // 将每个节点的部分结果发送给主节点
    std::vector<int> all_votes(num_processes * 4);
    MPI_Gather(votes_per_party.data(), 4, MPI_INT, all_votes.data(), 4, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // 汇总所有分散的子任务的结果
        for (int i = 0; i < num_processes; i++) {
            for (int j = 0; j < 4; j++) {
                votes_per_party[j] += all_votes[i*4+j];
            }
        }

        // 计算每个政党的总票数并输出结果
        int total_votes = 0;
        for (int i = 0; i < 4; i++) {
            total_votes += votes_per_party[i];
            std::cout << "Party " << i << " received " << votes_per_party[i] << " votes" << std::endl;
        }
        std::cout << "Total votes: " << total_votes << std::endl;
    }

    MPI_Finalize();
    return 0;
}
