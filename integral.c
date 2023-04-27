#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define f(x) (x*x)  // 定义被积函数

double trapezoidal_rule(double a, double b, int n, double h) {
    // 计算积分并返回结果
    double sum = 0.5 * (f(a) + f(b));
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return h * sum;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 0.0, b = 1.0; // 积分区间 [a, b]
    int n = 10000; // 分段数
    double h = (b - a) / (double) n;
    double local_a, local_b, local_result = 0.0, result;

    if (rank == 0) {
        // 获取用户输入的积分区间和分段数
        printf("Enter the interval [a, b]: ");
        scanf("%lf %lf", &a, &b);

        printf("Enter the number of intervals: ");
        scanf("%d", &n);
    }

    // 广播积分区间和分段数
    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 计算每个进程分配到的积分区间
    int local_n = n / size;
    local_a = a + rank * local_n * h;
    local_b = local_a + local_n * h;

    // 计算每个进程的局部积分结果
    local_result = trapezoidal_rule(local_a, local_b, local_n, h);

    // 将所有进程的局部积分结果汇总
    MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 输出积分结果
    if (rank == 0) {
        printf("The integral of f(x) from %lf to %lf is: %lf\n", a, b, result);
    }

    MPI_Finalize();
    return 0;
}

