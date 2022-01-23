import numpy as np

N1 = 10

data1 = np.random.randint(-10, 10, size=(N1, N1))
data2 = np.random.randint(-10, 10, size=(N1, N1))

np.savetxt('a.txt', data1, fmt='%10.0f')
np.savetxt('b.txt', data2, fmt='%10.0f')