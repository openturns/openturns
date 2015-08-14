#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # Read the wrapper file named 'poutre'
    wrapper = WrapperFile.FindWrapperByName("poutre")

    # Show where the wrapper was found
    # print wrapper.getDescriptionFilePath()

    # Get the content of the wrapper file
    data = WrapperData(wrapper.getWrapperData())
    filelist = data.getFileList()
    varlist = data.getVariableList()
    parameters = data.getParameters()

    # Show the content
    # replace dll suffix on windows
    libraryPath = data.getLibraryPath().replace(".dll", ".so")
    print("library path =", libraryPath)
    print("function =", repr(data.getFunctionDescription()))
    print("gradient =", repr(data.getGradientDescription()))
    print("hessian  =", repr(data.getHessianDescription()))
    print("Files:")
    for f in filelist:
        print("file ->", repr(f))
    print("Variables:")
    for v in varlist:
        print("variable ->", repr(v))
    print("parameters =", repr(parameters))

    # Add an (useless) new variable to the description
    V = WrapperDataVariable()
    V.id_ = "V"
    V.comment_ = "Useless variable"
    V.unit_ = "None"
    V.regexp_ = "V=.*"
    V.format_ = "V=%10.5g"
    V.type_ = 0  # 0: in, 1:out
    varlist.add(V)
    print("New variables:")
    for v in varlist:
        print("variable ->", repr(v))
    data.setVariableList(varlist)

    # Add an (useless) new file to the description
    F = WrapperDataFile()
    F.id_ = "Fich"
    F.name_ = "Useless file"
#  F.path_  = "c:/uselessFile"
    F.path_ = "/tmp/uselessFile"
    F.type_ = 1  # 0:in, 1:out
    F.subst_ = "V"
    filelist.add(F)
    print("New files:")
    for f in filelist:
        print("file ->", repr(f))
    data.setFileList(filelist)

    # Update the wrapper description with the new data and
    # write it out to disk
    if (data.isValid()):
        wrapper.setWrapperData(data)
        print("data =", data.__repr__().replace(".dll", ".so"))
        wrapper.writeFile("wrp.xml")

    deviation = NumericalMathFunction(wrapper)
    outPoint = deviation((210.e9, 1000, 1.5, 2.e-6, 77777))
    print("deviation =", outPoint)

except:
    import sys
    print("t_WrapperFile_std.py", sys.exc_info()[0], sys.exc_info()[1])
