from BandMatrix import BandMatrix
import numpy as np

N = 7

# TODO: vary these in tests for mapping test
l = 2
u = 2

def create_test_data():
    print "\nCreating the data"

    A = np.zeros((N,N))
    A[1,0] = 0.1; A[1,2] = 0.2; A[1,3] = 0.3
    A[2,1] = 0.4
    A[3,1] = 0.5; A[3,4] = 0.6
    A[4,3] = .7; A[4,5] = .8; A[4,6] = .9
    A[5,4] = 1
    A[6,4] = 1.1
    return A

def test_equivalence():
    print "\nTesting the equivalence"
    A = create_test_data()
    B = BandMatrix(A, l, u, "C")
    B_ = BandMatrix(A, l, u, "R")

    print "A is:\n{}".format(A)
    print "B is:\n{}".format(B)
    print "B_ is:\n{}".format(B_)

    for row in xrange(A.shape[0]):
        for col in xrange(A.shape[1]):
            if (A[row, col] != B[row, col]):
                print "IndexError row = {}, col = {}. A = {}, B = {}\n".\
                        format(row, col, A[row, col], B[row, col])
                exit(911)

    for row in xrange(A.shape[0]):
        for col in xrange(A.shape[1]):
            if (A[row, col] != B_[row, col]):
                print "IndexError row = {}, col = {}. A = {}, B = {}\n".\
                        format(row, col, A[row, col], B_[row, col])
                exit(911)
    print "Completed Test equivalence SUCCESSfully"

def test_mv():
    print "\nTesting Matrix/Vector multiplication"
    A = create_test_data()
    x = np.random.random((N,1))
    B = BandMatrix(A, l, u, "R")

    AC = np.dot(A,x)
    BC = B*x

    # Verbose
    print "AC:\n", AC
    print "BC:\n", BC

    assert np.all(np.isclose(AC.flatten(), BC.flatten()))

def main():
    test_equivalence()
    test_mv()

if __name__ == "__main__":
    main()
