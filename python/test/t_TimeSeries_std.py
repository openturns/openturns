#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


dim = 2

# We create an empty series
ts1 = ot.TimeSeries(0, dim)
ts1.setName('Ts1')

# We populate the empty ts
for p in range(3):
    pt = ot.Point(dim)
    for i in range(dim):
        pt[i] = 10. * (p + 1) + i
    ts1.add(pt)

print('ts1=', ts1)
print('len=', len(ts1))

# We get the second element of the ts
secondElement = ts1[1]
print('second element=', secondElement)

# We set the third element to a valid new element
newPoint = ot.Point(dim + 1)
for i in range(dim):
    newPoint[i + 1] = 1000. * (i + 1)
ts1[2] = newPoint
print('ts1=', ts1)

# We get the values of the second element of the ts
values1 = ts1.getValueAtIndex(1)
print('values of the second element=', values1)

# We set new values for the second element of the ts
newValues = values1 * 2.
ts1.setValueAtIndex(1, newValues)
values2 = ts1.getValueAtIndex(1)
print('ts1=', ts1)
print('modified values of the second element=',
      values2, ' should be ', newValues)

# We get the values of the second element of the ts
tree = ot.KDTree(ts1.getMesh().getVertices())
values3 = ts1.getValueAtIndex(tree.query([-1.0]))
print('values at t=-1.0 =', values3)
values4 = ts1.getValueAtIndex(tree.query([1.45]))
print('values at t=1.45 =', values4)
values5 = ts1.getValueAtIndex(tree.query([1.54]))
print('values at t=1.54 =', values5)
values6 = ts1.getValueAtIndex(tree.query([14.5]))
print('values at t=14.5 =', values6)

# We set new values for the third element of the ts
ts1.setValueAtIndex(tree.query([1.54]), values6 * -1.0)
print('ts1=', ts1)

ts2 = ot.TimeSeries(10, dim)
print('ts2=', ts2)

# ts2[5] = point2
# print 'ts2=', ts2

try:
    # We get the tenth element of the ts
    # THIS SHOULD NORMALLY FAIL
    tenthElement = ts1.at(9)
except:
    print('Expected failure')

tg1 = ot.RegularGrid(0.0, 0.1, 11)
ts3 = ot.TimeSeries(tg1, dim)
print('ts3=', ts3)

tg2 = ot.RegularGrid(0.0, 0.2, 6)
ts4 = ot.TimeSeries(tg2, dim)
print('ts4=', ts4)

# We append a sample to a time series
ts5 = ot.TimeSeries(3, dim)
ns1 = ot.Sample(3, [99.9] * dim)
print('ts5=', ts5)
ts5.add(ns1)
print('ts5=', ts5)

# We retrieve the values of the time series as a sample
ns2 = ts5.getValues()
print('ns2=', ns2)

# We build a time series from a time grid and a sample
ns3 = ts4.getValues()
ts6 = ot.TimeSeries(tg2, ns3)
print('ts6=', ts6)
