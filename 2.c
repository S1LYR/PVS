#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>  // Добавляем для memcmp

#define N 100000000  // 100 миллионов элементов
#define RANGE 1000   // Диапазон значений: 0..999

// Последовательная быстрая сортировка
void sequential_quicksort(int *arr, int left, int right) {
    if (left >= right) return;
    
    int pivot = arr[(left + right) / 2];
    int i = left, j = right;
    
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }
    
    sequential_quicksort(arr, left, j);
    sequential_quicksort(arr, i, right);
}

// Параллельная быстрая сортировка
void parallel_quicksort(int *arr, int left, int right, int threads) {
    if (left >= right) return;
    
    int pivot = arr[(left + right) / 2];
    int i = left, j = right;
    
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }
    
    #pragma omp parallel sections num_threads(threads)
    {
        #pragma omp section
        parallel_quicksort(arr, left, j, threads);
        #pragma omp section
        parallel_quicksort(arr, i, right, threads);
    }
}

// Проверка отсортированности
int is_sorted(int *arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

void benchmark() {
    int *array_seq = malloc(N * sizeof(int));
    int *array_par = malloc(N * sizeof(int));
    if (!array_seq || !array_par) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Инициализация массива случайными числами
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        array_seq[i] = array_par[i] = rand() % RANGE;
    }

    // Эталонная последовательная сортировка
    double seq_start = omp_get_wtime();
    sequential_quicksort(array_seq, 0, N - 1);
    double seq_time = omp_get_wtime() - seq_start;
    int seq_correct = is_sorted(array_seq, N);

    printf("Array size: %d elements | Range: 0..%d\n\n", N, RANGE - 1);
    printf("Mode       | Threads | Time (sec) | Speedup | Correct\n");
    printf("-----------+---------+------------+---------+--------\n");
    printf("Sequential |      1  | %9.5f  |  1.00x  |   %s\n", 
           seq_time, seq_correct ? "yes" : "NO!");

    // Тестируем параллельные версии
    int thread_counts[] = {2, 4, 8, 16};
    for (int i = 0; i < 4; i++) {
        int threads = thread_counts[i];
        
        // Восстанавливаем несортированный массив
        for (int j = 0; j < N; j++) {
            array_par[j] = array_seq[j];
        }
        
        double par_start = omp_get_wtime();
        parallel_quicksort(array_par, 0, N - 1, threads);
        double par_time = omp_get_wtime() - par_start;
        int par_correct = is_sorted(array_par, N) && 
                         (memcmp(array_seq, array_par, N * sizeof(int)) == 0);
        
        printf("Parallel   |    %2d   | %9.5f  | %6.2fx  |   %s\n",
               threads, par_time, seq_time / par_time,
               par_correct ? "yes" : "NO!");
    }

    free(array_seq);
    free(array_par);
}

int main() {
    printf("Available CPU cores: %d\n", omp_get_num_procs());
    benchmark();
    return 0;
}