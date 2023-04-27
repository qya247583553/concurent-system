#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double f1(double x) {
    // 第一个被积函数
    return x*x;
}

double f2(double x) {
    // 第二个被积函数
    return sqrt(1 - x*x);
}

double (*f)(double); // 指向被积函数的指针

double trapezoidal_rule(double a, double b, int n, double h) {
    // 计算积分并返回结果
    double sum = 0.5 * (f(a) + f(b));
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return h * sum;
}

void get_user_input(double* a, double* b, int* n, int* choice, int rank) {
    if (rank == 0) {
        printf("Select the function to integrate:\n");
        printf("1. f(x) = x^2\n");
        printf("2. f(x) = sqrt(1 - x^2)\n");
        scanf("%d", choice);

        printf("Enter the interval [a, b]: ");
        scanf("%lf %lf", a, b);

        printf("Enter the number of intervals: ");
        scanf("%d", n);
    }

    // 广播积分区间和分段数
    MPI_Bcast(a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 根据用户选择设置被积函数
    switch (*choice) {
        case 1:
            f = f1;
            break;
        case 2:
            f = f2;
            break;
        default:
            printf("Invalid choice!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 0.0, b = 1.0; // 积分区间 [a, b]
    int n = 10000; // 分段数
    int choice; // 用户选择的被积函数
    double h = (b - a) / (double) n;
    double local_a, local_b, local_result = 0.0, result;

    get_user_input(&a, &b, &n, &choice, rank);

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


    