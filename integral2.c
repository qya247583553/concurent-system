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

double simpson_rule(double a, double b, int n, double h) {
    // 计算积分并返回结果
    double sum = f(a) + f(b);
    for (int i = 1; i < n; i += 2) {
        double x = a + i * h;
        sum += 4 * f(x);
    }
    for (int i = 2; i < n; i += 2) {
        double x = a + i * h;
        sum += 2 * f(x);
    }
    return h * sum / 3.0;
}

void get_user_input(double* a, double* b, int* n, int* choice, int rank) {
    if (rank == 0) {
        printf("1.Select the function to integrate: 2. Enter the interval [a, b]: 3. Enter the number of intervals: \n");
        printf("1. f(x) = x^2\n");
        printf("2. f(x) = sqrt(1 - x^2)\n");
        scanf("%d %lf %lf %d", choice,a,b,n);

       

        // 如果分段数是奇数，将其增加为偶数
        if (*n % 2 != 0) {
            *n += 1;
        }
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
    int choice; // 用户选择的被
        double h = (b - a) / (double) n;
    double local_a, local_b, local_result_trapezoidal = 0.0, local_result_simpson = 0.0;
    double result_trapezoidal, result_simpson;

    get_user_input(&a, &b, &n, &choice, rank);

    // 计算每个进程分配到的积分区间
    int local_n = n / size;
    local_a = a + rank * local_n * h;
    local_b = local_a + local_n * h;

    // 处理最后一个进程所处理的区间
    if (rank == size - 1) {
        local_n += n % size;
        local_b = b;
    }

    // 计算每个进程的局部积分结果
    local_result_trapezoidal = trapezoidal_rule(local_a, local_b, local_n, h);
    local_result_simpson = simpson_rule(local_a, local_b, local_n, h);

    // 将所有进程的局部积分结果汇总
    MPI_Reduce(&local_result_trapezoidal, &result_trapezoidal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_result_simpson, &result_simpson, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 输出积分结果
    if (rank == 0) {
        printf("Using Trapezoidal rule:\n");
        printf("The choice is %d The integral of f(x) from %lf to %lf is: %lf The N is %d\n", a, b, result_trapezoidal);

        printf("\nUsing Simpson's rule:\n");
        printf("The choice is %d The integral of f(x) from %lf to %lf is: %lf The N is %d\n", a, b, result_simpson);
    }

    MPI_Finalize();
    return 0;
}


    