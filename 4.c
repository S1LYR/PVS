#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define ROWS 1000    // Количество строк
#define COLS 1000    // Количество столбцов
#define RANGE 100    // Диапазон значений: 1..100

// Последовательные операции
void sequential_operations(double **a, double **b, double **sum, double **diff, 
                         double **prod, double **quot) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            sum[i][j] = a[i][j] + b[i][j];
            diff[i][j] = a[i][j] - b[i][j];
            prod[i][j] = a[i][j] * b[i][j];
            quot[i][j] = a[i][j] / b[i][j];
        }
    }
}

// Параллельные операции
void parallel_operations(double **a, double **b, double **sum, double **diff,
                       double **prod, double **quot, int threads) {
    #pragma omp parallel for num_threads(threads) collapse(2)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            sum[i][j] = a[i][j] + b[i][j];
            diff[i][j] = a[i][j] - b[i][j];
            prod[i][j] = a[i][j] * b[i][j];
            quot[i][j] = a[i][j] / b[i][j];
        }
    }
}

// Проверка корректности
int verify_results(double **seq_sum, double **seq_diff, double **seq_prod, double **seq_quot,
                  double **par_sum, double **par_diff, double **par_prod, double **par_quot) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (fabs(seq_sum[i][j] - par_sum[i][j]) > 1e-9 ||
                fabs(seq_diff[i][j] - par_diff[i][j]) > 1e-9 ||
                fabs(seq_prod[i][j] - par_prod[i][j]) > 1e-9 ||
                fabs(seq_quot[i][j] - par_quot[i][j]) > 1e-9) {
                return 0;
            }
        }
    }
    return 1;
}

// Выделение памяти для двумерного массива
double** allocate_matrix() {
    double **matrix = (double**)malloc(ROWS * sizeof(double*));
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = (double*)malloc(COLS * sizeof(double));
    }
    return matrix;
}

// Освобождение памяти
void free_matrix(double **matrix) {
    for (int i = 0; i < ROWS; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void benchmark() {
    // Выделение памяти
    double **a = allocate_matrix();
    double **b = allocate_matrix();
    double **seq_sum = allocate_matrix();
    double **seq_diff = allocate_matrix();
    double **seq_prod = allocate_matrix();
    double **seq_quot = allocate_matrix();
    double **par_sum = allocate_matrix();
    double **par_diff = allocate_matrix();
    double **par_prod = allocate_matrix();
    double **par_quot = allocate_matrix();

    // Инициализация массивов
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            a[i][j] = 1.0 + (i * COLS + j) % RANGE;
            b[i][j] = 1.0 + ((i * COLS + j + 1) % RANGE);
        }
    }

    // Последовательные операции
    double seq_start = omp_get_wtime();
    sequential_operations(a, b, seq_sum, seq_diff, seq_prod, seq_quot);
    double seq_time = omp_get_wtime() - seq_start;

    printf("Matrix size: %dx%d (%d elements) | Operations: + - * /\n\n", 
           ROWS, COLS, ROWS*COLS);
    printf("Mode       | Threads | Time (sec) | Speedup | Correct\n");
    printf("-----------+---------+------------+---------+--------\n");
    printf("Sequential |      1  | %9.5f  |  1.00x  |   yes\n", seq_time);

    // Параллельные операции
    int thread_counts[] = {2, 4, 8, 16};
    for (int i = 0; i < 4; i++) {
        int threads = thread_counts[i];
        
        double par_start = omp_get_wtime();
        parallel_operations(a, b, par_sum, par_diff, par_prod, par_quot, threads);
        double par_time = omp_get_wtime() - par_start;
        
        int correct = verify_results(seq_sum, seq_diff, seq_prod, seq_quot,
                                   par_sum, par_diff, par_prod, par_quot);
        printf("Parallel   |    %2d   | %9.5f  | %6.2fx  |   %s\n",
               threads, par_time, seq_time / par_time,
               correct ? "yes" : "NO!");
    }

    // Освобождение памяти
    free_matrix(a); free_matrix(b);
    free_matrix(seq_sum); free_matrix(seq_diff);
    free_matrix(seq_prod); free_matrix(seq_quot);
    free_matrix(par_sum); free_matrix(par_diff);
    free_matrix(par_prod); free_matrix(par_quot);
}

int main() {
    printf("Available CPU cores: %d\n", omp_get_num_procs());
    benchmark();
    return 0;
}