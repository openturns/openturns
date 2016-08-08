#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:
    print('Catalog={')
    for key in ot.Catalog.GetKeys():
        print('  %s,' % key)
    print('}')

except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
