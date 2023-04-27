#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double f(double x) {
    // The function to be integrated
    return x * x;
}

double trapezoidal(double a, double b, int n) {
    // Calculate the integral using the trapezoidal rule
    double h = (b - a) / (double) n;
    double sum = 0.5 * (f(a) + f(b));
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }
    return h * sum;
}

double simpson(double a, double b, int n) {
    // Calculate the integral using Simpson's rule
    double h = (b - a) / (double) n;
    double sum = f(a) + f(b);
    for (int i = 1; i < n; i += 2) {
        double x = a + i * h;
        sum += 4 * f(x);
    }
    for (int i = 2; i < n; i += 2) {
        double x = a + i * h;
        sum += 2 * f(x);
    }
    return (h / 3.0) * sum;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 0.0, b = 1.0; // The interval [a, b]
    int n = 10000; // The number of intervals
    double result_trap, result_simpson;

    if (rank == 0) {
        // Get the user's input for the function, interval, and number of intervals
        printf("Enter the function number (1: x^2, 2: sin(x), 3: exp(x)): ");
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 2:
                f = sin;
                break;
            case 3:
                f = exp;
                break;
            default:
                f = f;
                break;
        }

        printf("Enter the interval [a, b]: ");
        scanf("%lf %lf", &a, &b);

        printf("Enter the number of intervals: ");
        scanf("%d", &n);
    }

    // Broadcast the parameters
    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate the integral using the trapezoidal rule and measure the time
    double start, end;
    start = MPI_Wtime();
    result_trap = trapezoidal(a, b, n);
    end = MPI_Wtime();
    double time_trap = end - start;

    // Calculate the integral using Simpson's rule and measure the time
    start = MPI_Wtime
    result_simpson = simpson(a, b, n);
    end = MPI_Wtime();
    double time_simpson = end - start;

    // Reduce the results and calculate the average
    double total_trap, total_simpson;
    MPI_Reduce(&result_trap, &total_trap, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&result_simpson, &total_simpson, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
    // Calculate the average of the integral values
    double avg_trap = total_trap / size;
    double avg_simpson = total_simpson / size;

    // Print the results
    printf("Trapezoidal rule:\n");
    printf("Function number: %d\n", f == sin ? 2 : f == exp ? 3 : 1);
    printf("Interval: [%lf, %lf]\n", a, b);
    printf("Number of intervals: %d\n", n);
    printf("Calculated integral: %lf\n", avg_trap);
    printf("Running time: %lf seconds\n\n", time_trap);

    printf("Simpson's rule:\n");
    printf("Function number: %d\n", f == sin ? 2 : f == exp ? 3 : 1);
    printf("Interval: [%lf, %lf]\n", a, b);
    printf("Number of intervals: %d\n", n);
    printf("Calculated integral: %lf\n", avg_simpson);
    printf("Running time: %lf seconds\n", time_simpson);
}

MPI_Finalize();
return 0;
