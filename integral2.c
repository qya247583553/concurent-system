#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double f1(double x) {
    // First integrand
    return x*x;
}

double f2(double x) {
    // second integrand
    return 1 - x*x;
}

double (*f)(double); // poinnter to the integrand

double trapezoidal_rule(double a, double b, int n, double h) {
    // calculate the integral and return results
//first method
    double sum = 0.5 * (f(a) + f(b));
    int i;
    for ( i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return h * sum;
}

double simpson_rule(double a, double b, int n, double h) {
    // calculate the integral and return results
//second method
    double sum = f(a) + f(b);
    int i;
    for ( i = 1; i < n; i += 2) {
        double x = a + i * h;
        sum += 4 * f(x);
    }
    for ( i = 2; i < n; i += 2) {
        double x = a + i * h;
        sum += 2 * f(x);
    }
    return h * sum / 3.0;
}
//scan the input choose
void get_user_input(double* a, double* b, int* n, int* choice, int rank) {
    if (rank == 0) {
        printf("1.Select the function to integrate: 2. Enter the interval [a, b]: 3. Enter the number of intervals: \n");
        printf("1. f(x) = x^2\n");
        printf("2. f(x) = 1 - x^2\n");
        scanf("%d %lf %lf %d", choice,a,b,n);

       

        // if the number of segements is odd force it to be even
        if (*n % 2 != 0) {
            *n += 1;
        }
    }

//Broadcast a b n and choice to whole process
    MPI_Bcast(a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // choose difference iput user choice
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

    double a = 0.0, b = 1.0; // integral space [a, b]
    int n = 10000; // the number of n
    int choice; // user input
        double h = (b - a) / (double) n;
    double local_a, local_b, local_result_trapezoidal = 0.0, local_result_simpson = 0.0;
    double result_trapezoidal, result_simpson;

    get_user_input(&a, &b, &n, &choice, rank);

  
//calculate the integration intevals to which each interval is assigned
    int local_n = n / size;
    local_a = a + rank * local_n * h;
    local_b = local_a + local_n * h;

//compute the final process interval
    if (rank == size - 1) {
        local_n += n % size;
        local_b = b;
    }

//compute each process integral part result
    local_result_trapezoidal = trapezoidal_rule(local_a, local_b, local_n, h);
    local_result_simpson = simpson_rule(local_a, local_b, local_n, h);

    // let whole procee result together
    MPI_Reduce(&local_result_trapezoidal, &result_trapezoidal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_result_simpson, &result_simpson, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

//output results
    if (rank == 0) {
        printf("Using Trapezoidal rule:\n");
        printf("The choice is %d The integral of f(x) from %lf to %lf is: %lf The N is %d\n", choice,a, b, result_trapezoidal,n);

        printf("\nUsing Simpson's rule:\n");
        printf("The choice is %d The integral of f(x) from %lf to %lf is: %lf The N is %d\n",choice, a, b, result_simpson,n);
    }

    MPI_Finalize();
    return 0;
}
