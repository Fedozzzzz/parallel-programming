from mpi4py import MPI
import time
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
num_of_proc = comm.Get_size()
workers = num_of_proc - 1

mtrxA = np.array([])
mtrxB = np.array([])
mtrxC = np.array([])

N = 1000


def init_matrices():
    global mtrxA
    mtrxA = np.loadtxt("a.txt")

    global mtrxB
    mtrxB = np.loadtxt("b.txt")


def multiply(A, B):
    return np.array([[sum(a * b for a, b in zip(X_row, Y_col)) for Y_col in zip(*B)] for X_row in A])


def master_job():
    start_i = int(N / num_of_proc)
    step = start_i
    rows = np.split(mtrxA, [n for n in range(start_i, N, step)])

    row_m = np.array(rows[0])
    rows = np.delete(rows, 0, axis=0)

    pid = 1
    for row in rows:
        comm.send(row, dest=pid, tag=1)
        comm.send(mtrxB, dest=pid, tag=2)
        pid += 1

    print("Computation executes in MASTER...")
    temp = multiply(row_m, mtrxB)
    global mtrxC
    print("Computation finished in MASTER!")

    pid = 1
    np.append(mtrxC, temp)
    for n in range(workers):
        row = comm.recv(source=pid, tag=pid)
        print("Result received in MASTER from Slave#{}!".format(pid))
        np.append(mtrxC, row)
        pid += 1


def slave_job():
    x = comm.recv(source=0, tag=1)
    y = comm.recv(source=0, tag=2)
    print("Computation executes in SLAVE...")
    z = multiply(x, y)
    print("Computation finished in SLAVE!")
    comm.send(z, dest=0, tag=rank)


if __name__ == '__main__':
    if rank == 0:
        start_time = time.time()
        init_matrices()

        print("-------------------------------------------------")
        print("Start time:{}".format(start_time))
        print("-------------------------------------------------")

        master_job()

        np.savetxt('c_mpi_python.txt', mtrxC, fmt='%10.0f')

        end_time = time.time()
        print("-------------------------------------------------")
        print("End time:{}".format(end_time))
        print("-------------------------------------------------")
        print('Time:{}s'.format(end_time - start_time))
    else:
        slave_job()
