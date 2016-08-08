#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:
    print('ResourceMap={')
    for key in ot.ResourceMap.GetKeys():
        print('  %s => %s,' % (key, ot.ResourceMap.Get(key)))
    print('}')
    print('Extract from ResourceMap : R-executable-command -> ',
          ot.ResourceMap.Get('R-executable-command'))
except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
