#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>

int* a, * b, * c, * buffer, * result;
int size, line, numprocs;
FILE* fp;


void* init_matrices();

void* write_result_to_file();

void* master_job();

void* slave_job();

int main(int argc, char* argv[])
{
    double start_time, end_time;
    int i, j, k, l;
    size = 10;

    int rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    line = size / numprocs;
    a = (int*)malloc(sizeof(int) * size * size);
    b = (int*)malloc(sizeof(int) * size * size);
    c = (int*)malloc(sizeof(int) * size * size);

    buffer = (int*)malloc(sizeof(int) * size * line);
    result = (int*)malloc(sizeof(int) * size * line);

    if (rank == 0)
    {
        start_time = MPI_Wtime();

        init_matrices();

        master_job();

        write_result_to_file();

        end_time = MPI_Wtime();

        printf("rank:%d time:%lfs\n", rank, end_time - start_time);

        free(a);
        free(b);
        free(c);
        free(buffer);
        free(result);
    }
    else
    {
        slave_job();
    }

    MPI_Finalize();

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
}

void* write_result_to_file() {
    int i, j;

    fp = fopen("c_mpi_c.txt", "w");

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            fprintf(fp, "%d ", c[i * size + j]);
        }
        fputc('\n', fp);
    }
    fclose(fp);
}

void* master_job() {
    int i, j, k, l;

    for (i = 1; i < numprocs; i++) {
        MPI_Send(b, size * size, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    for (l = 1; l < numprocs; l++) {
        MPI_Send(a + (l - 1) * line * size, size * line, MPI_INT, l, 1, MPI_COMM_WORLD);
    }
    for (k = 1; k < numprocs; k++) {
        MPI_Recv(result, line * size, MPI_INT, k, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (i = 0; i < line; i++) {
            for (j = 0; j < size; j++) {
                c[((k - 1) * line + i) * size + j] = result[i * size + j];
            }
        }
    }
    for (i = (numprocs - 1) * line; i < size; i++) {
        for (j = 0; j < size; j++) {
            int temp = 0;
            for (k = 0; k < size; k++) {
                temp += a[i * size + k] * b[k * size + j];
            }
            c[i * size + j] = temp;
        }
    }
}

void* slave_job() {
    int i, j, k;

    MPI_Recv(b, size * size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv(buffer, size * line, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (i = 0; i < line; i++)
    {
        for (j = 0; j < size; j++)
        {
            int temp = 0;
            for (k = 0; k < size; k++)
                temp += buffer[i * size + k] * b[k * size + j];
            result[i * size + j] = temp;
        }
    }
    MPI_Send(result, line * size, MPI_INT, 0, 3, MPI_COMM_WORLD);
}