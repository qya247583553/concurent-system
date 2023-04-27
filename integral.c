#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

double function(double x){
    // The function to integrate
    return x*x;
}

double trapezoidal(double a, double b, int n){
    // Integration using the trapezoidal rule
    double h = (b-a)/(double)n;
    double sum = 0.5*(function(a)+function(b));
    for(int i = 1; i < n; i++){
        double x = a + i*h;
        sum += function(x);
    }
    return h*sum;
}

double monte_carlo(double a, double b, int n){
    // Integration using Monte Carlo integration
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL)+rank);

    int count = 0;
    for(int i = 0; i < n; i++){
        double x = ((double)rand()/RAND_MAX)*(b-a) + a;
        double y = ((double)rand()/RAND_MAX)*1.0;
        if(y < function(x)){
            count++;
        }
    }

    int total_count;
    MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double area = (b-a)*1.0*(double)total_count/n;
    return area;
}

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 0.0, b = 1.0; // interval [a, b]
    int n = 100000; // number of intervals
    double result_trap, result_mc;

    // Calculate integral using trapezoidal rule
    double start = MPI_Wtime();
    result_trap = trapezoidal(a, b, n);
    double end = MPI_Wtime();
    double time_trap = end - start;

    // Calculate integral using Monte Carlo integration
    start = MPI_Wtime();
    result_mc = monte_carlo(a, b, n);
    end = MPI_Wtime();
    double time_mc = end - start;

    if(rank == 0){
        printf("Trapezoidal Rule:\n");
        printf("Interval: [%lf, %lf]\n", a, b);
        printf("Number of intervals: %d\n", n);
        printf("Computed Integral: %lf\n", result_trap);
        printf("Running time: %lf seconds\n\n", time_trap);

        printf("Monte Carlo Integration:\n");
        printf("Interval: [%lf, %lf]\n", a, b);
        printf("Number of intervals: %d\n", n);
        printf("Computed Integral: %lf\n", result_mc);
        printf("Running time: %lf seconds\n", time_mc);
    }

    MPI_Finalize();
    return 0;
}
