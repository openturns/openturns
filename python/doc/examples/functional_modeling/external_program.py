
# -*- coding: utf-8 -*-

# 1. Get input
import sys
inFile = sys.argv[1]
exec(open(inFile).read())
 
# 2. Compute
Y0 = X0 + X1 + X2
Y1 = X0 + X1 * X2
 
# 3. Write output
f = open("output.txt", "w")
f.write("Y0=%.17e\n" % (Y0))
f.write("Y1=%.17e\n" % (Y1))
f.close()
