import numpy as np


def test_pthread(expected_c):
    actual_c = np.loadtxt("c_pthread.txt")
    if np.array_equal(actual_c, expected_c):
        print("test_pthread COMPLETED")
    else:
        print("test_pthread FAILED")


def test_mpi_c(expected_c):
    actual_c = np.loadtxt("c_mpi_c.txt")
    if np.array_equal(actual_c, expected_c):
        print("test_mpi_c COMPLETED")
    else:
        print("test_mpi_c FAILED")


def test_mpi_python(expected_c):
    actual_c = np.loadtxt("c_mpi_python.txt")
    if np.array_equal(actual_c, expected_c):
        print("test_mpi_python COMPLETED")
    else:
        print("test_mpi_python FAILED")

def test_openmp(expected_c):
    actual_c = np.loadtxt("c_openmp.txt")
    if np.array_equal(actual_c, expected_c):
        print("test_openmp COMPLETED")
    else:
        print("test_openmp FAILED")


if __name__ == '__main__':
    matr_a = np.loadtxt("a.txt")
    matr_b = np.loadtxt("b.txt")
    expected_c = np.dot(matr_a, matr_b)

    test_pthread(expected_c)
    test_mpi_c(expected_c)
    test_mpi_python(expected_c)
    test_openmp(expected_c)
