#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

print('ResourceMap={')
for key in ot.ResourceMap.GetKeys():
    print('  %s => %s,' % (key, ot.ResourceMap.Get(key)))
print('}')
print('Extract from ResourceMap : R-executable-command -> ',
      ot.ResourceMap.Get('Graph-RExecutableCommand'))
