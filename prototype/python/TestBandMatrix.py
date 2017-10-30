from BandMatrix import BandMatrix
import numpy as np

def test_equivalence():
    N = 7
    A = np.zeros((N,N))
    A[1,0] = 0.1; A[1,2] = 0.2; A[1,3] = 0.3
    A[2,1] = 0.4
    A[3,1] = 0.5; A[3,4] = 0.6
    A[4,3] = .7; A[4,5] = .8; A[4,6] = .9
    A[5,4] = 1
    A[6,4] = 1.1

    B = BandMatrix(A, 2, 2)

    print "A is:\n{}".format(A)
    print "B is:\n{}".format(B)

    for row in xrange(A.shape[0]):
        for col in xrange(A.shape[1]):
            if (A[row, col] != B[row, col]):
                print "IndexError row = {}, col = {}. A = {}, B = {}\n".\
                        format(row, col, A[row, col], B[row, col])
                exit(911)

def test_mv():
    pass

def main():
    test_equivalence()

if __name__ == "__main__":
    main()
