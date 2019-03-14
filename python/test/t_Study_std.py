#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import os
import inspect

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)

# find all instanciable classes
instanciables = []
for name, obj in inspect.getmembers(ot):
    if inspect.isclass(obj):
        cn = obj.__name__
        if '_' in cn or 'Interface' in cn or 'Pointer' in cn:
            continue
        try:
            instance = obj()
            print (obj.__name__, 'OK')
            instanciables.append(obj)
        except Exception:
            print (obj.__name__, 'FAIL')

# save / load
fileName = 'myStudy.xml'
for class_ in instanciables:
    myStudy = ot.Study()
    myStudy.setStorageManager(ot.XMLStorageManager(fileName))

    print (class_.__name__)
    try:
        instance = class_()
        myStudy.add(class_.__name__, instance)
        myStudy.save()
        myStudy = ot.Study()
        myStudy.setStorageManager(ot.XMLStorageManager(fileName))
        myStudy.load()
        os.remove(fileName)
        instance = class_()
        myStudy.fillObject(class_.__name__, instance)
        print (class_.__name__, 'OK')
    except Exception as exc:
        print('--', class_.__name__, exc)
