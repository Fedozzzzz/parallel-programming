#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<sys/time.h>

int thread_count;
int size, local_size;
int* a, * b, * c;
FILE* fp;

void* init_matrices();

void* write_result_to_file();

void mult();

int main(int argc, char* argv[]) {
    int i, j;

    float time_use = 0;
    struct timeval start_time;
    struct timeval end_time;

    size = 10;

    a = (int*)malloc(sizeof(int) * size * size);
    b = (int*)malloc(sizeof(int) * size * size);
    c = (int*)malloc(sizeof(int) * size * size);

    gettimeofday(&start_time, NULL);

    thread_count = strtol(argv[1], NULL, 10);

    local_size = size / thread_count;

    init_matrices();

# pragma omp parallel num_threads(thread_count)
    mult();

    write_result_to_file();

    gettimeofday(&end_time, NULL);
    time_use = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);

    printf("time:%lfs\n", time_use / 1000000);

    free(a);
    free(b);
    free(c);

    return 0;
}

void* init_matrices() {
    int i, j;

    fp = fopen("a.txt", "r");
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            fscanf(fp, "%d", &a[i * size + j]);
    fclose(fp);

    fp = fopen("b.txt", "r");

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            fscanf(fp, "%d", &b[i * size + j]);
    fclose(fp);

    // transposition of matrix B
    for (i = 0; i < size; i++) {
        for (j = i + 1; j < size; j++) {
            int temp = b[i * size + j];
            b[i * size + j] = b[j * size + i];
            b[j * size + i] = temp;
        }
    }
}

void* write_result_to_file() {
    int i, j;

    fp = fopen("c_openmp.txt", "w");

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            fprintf(fp, "%d ", c[i * size + j]);
        }
        fputc('\n', fp);
    }
    fclose(fp);
}

void mult() {
    int it_rank = omp_get_thread_num();
    int i, j, k, temp;

    int first_row = it_rank * local_size;
    int last_row = (it_rank + 1) * local_size - 1;

    for (i = first_row; i <= last_row; i++) {
        for (j = 0; j < size; j++) {
            temp = 0;
            for (k = 0; k < size; k++)
                temp += a[i * size + k] * b[j * size + k];
            c[i * size + j] = temp;
        }

    }
}