#! /usr/bin/env python

from __future__ import print_function
from openturns import *


# We create an empty Matrix
N = 10
d = 5

matrices = [Matrix(N, d), ComplexMatrix(N, d)]
for matrix in matrices:

    # We populate the empty sample with numerous data
    for i in range(matrix.getNbRows()):
        for j in range(matrix.getNbColumns()):
            matrix[i, j] = 100. * (j + 1) + i
    print('matrix=', str(matrix))

    # We try to slice the sample

    # Reading slicing

    # case #0 [slice]
    print('rslice[:]=', matrix[:])
    print('rslice[2:4]=', matrix[2:4])
    print('rslice[1:8:2]=', matrix[1:8:2])

    # case #1 [slice/slice]
    print('rslice[:,:]=', matrix[:, :])
    print('rslice[2:4,2:4]=', matrix[2:4, 2:4])
    print('rslice[1:8:2,1:5:3]=', matrix[1:8:2, 1:5:3])

    # case #2 [slice/index]
    print('rslice[:,3]=', matrix[:, 3])
    print('rslice[2:7:2,3]=', matrix[2:7:2, 3])

    # case #3 [index/slice]
    print('rslice[3,:]=', matrix[3, :])
    print('rslice[3,1:4:2]=', matrix[3, 1:4:2])

    # case #4 [index/index]
    print('rslice[3,4]=', matrix[3, 4])

    # Writing slice

    # case #0 [slice]
    matrix2 = matrix
    matrix2[:] = matrix[:]
    print("wrslice[:]=", matrix2)
    matrix2 = matrix
    matrix2[2:4] = matrix[2:4]
    print('wslice[2:4]=', matrix2)
    matrix2 = matrix
    matrix2[1:8:2] = matrix[1:8:2]
    print('wslice[1:8:2]=', matrix2)

    # case #1 [slice/slice]
    matrix2 = matrix
    matrix2[:, :] = matrix[:, :]
    print("wrslice[:,:]=", matrix2)
    matrix2 = matrix
    matrix2[2:4, 2:4] = matrix[2:4, 2:4]
    print('wslice[2:4,2:4]=', matrix2)
    matrix2 = matrix
    matrix2[1:8:2, 1:5:3] = matrix[1:8:2, 1:5:3]
    print('wslice[1:8:2,1:5:3]=', matrix2)

    # case #2 [slice/index]
    matrix2 = matrix
    matrix2[:, 3] = matrix[:, 3]
    print('wslice[:,3]=', matrix2)
    matrix2 = matrix
    matrix2[2:7:2, 3] = matrix[2:7:2, 3]
    print('wslice[2:7:2,3]=', matrix2)

    # case #3 [index/slice]
    matrix2 = matrix
    matrix2[3, :] = matrix[3, :]
    print('wslice[3,:]=', matrix2)
    matrix2 = matrix
    matrix2[3, 1:4:2] = matrix[3, 1:4:2]
    print('wslice[3,1:4:2]=', matrix2)

    # case #4 [index/index]
    matrix2 = matrix
    matrix2[3, 4] = matrix[3, 4]
    print('wslice[3,4]=', matrix2)
