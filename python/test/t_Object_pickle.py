#! /usr/bin/env python

import openturns as ot
import pickle
from io import BytesIO

obj_list = []
obj_list.append(ot.Point([1.6, -8.7]))
obj_list.append(ot.Sample([[4.6, -3.7], [8.4, 6.3]]))
obj_list.append(ot.Description(['x', 'y', 'z']))
obj_list.append(ot.Indices([1, 2, 4]))
obj_list.append(ot.Matrix([[1, 2], [3, 4]]))
obj_list.append(ot.SymbolicFunction(['x1', 'x2'], ['y1=x1+x2']))

src = BytesIO()

for obj in obj_list:
    pickle.dump(obj, src)

src.seek(0)

for obj in obj_list:
    obj2 = pickle.load(src)
    print(('object: ' + str(obj)))
    print(('same: ' + str(obj2 == obj) + '\n'))
