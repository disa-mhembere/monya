import numpy as np
from time import time
import sys

class TreeBalancer(object):
    def __init__(self, n):
        self.left = np.array(range(n/2))
        self.right = np.array(range(n/2+1, n))
        self.ordered = [n/2]

    def get(self):

        while(self.left.shape[0] or self.right.shape[0]):
            if self.left.shape[0]:
                mid = len(self.left)/2 # integer div
                self.ordered.append(self.left[mid])
                self.left = np.delete(self.left, mid)
            if self.right.shape[0]:
                mid = len(self.right)/2 # integer div
                self.ordered.append(self.right[mid])
                self.right = np.delete(self.right, mid)

    def write(self,):
        np.array(self.ordered).tofile("ordered_tree.bin")

if __name__ == "__main__":
    print "Making halves ..."
    t = TreeBalancer(int(sys.argv[1]))

    print "Getting ordered ..."
    start = time()
    t.get()
    print "Time for getting ordered {} sec".format(time()-start)

    start = time()
    print "Writing ordered ..."
    t.write()
    print "Time for getting ordered {} sec".format(time()-start)
