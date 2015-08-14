#! /usr/bin/env python

from __future__ import print_function
import os
import sys
import openturns as ot

try:
    #Log.Show( Log.ALL )

    # Template input file
    in_file = 'inFILE_generic'
    f = open(in_file, 'w')
    f.write('F=10.\nE=5.\n')
    f.close()

    # Variables
    vars = ({"id_": "F",
             "type_": "in",
             "regexp_": "^\S*F\S*=\S*\R\S*$",
             "format_": "F=%20.13G"},

            {"id_": "E",
             "type_": "in",
             "regexp_": "^\S*E\S*=\S*\R\S*$",
             "format_": "E=%20.13G"},

            {"id_": "Z",
             "type_": "out",
             "regexp_": "^\S*Z\S*=\S*(\R)\S*$"},
            )

    # Files
    files = ({"id_": "infile",
              "type_": "in",
              "path_": "inFILE_generic",
              "subst_": "E,F"},

             {"id_": "outfile",
              "type_": "out",
              "path_": "outFILE_generic",
              "subst_": "Z"},
             )

    # Parameters
    params = {
        "command_": sys.executable + " -c \"exec(compile(open('inFILE_generic').read(), 'inFILE_generic', 'exec')); Z=E*F; print('Z=' + str(Z))\" > outFILE_generic",
        "userPrefix_": "GenericWrapperTest",
    }

    wrp = ot.makeWrapper(vars, files, params)

    # print wrp

    xml_file = 'wrp.xml'
    wrp.writeFile(xml_file)
    os.remove(xml_file)

    model = ot.NumericalMathFunction(wrp)

    inP = [101, 255]
    outP = model(inP)
    print(outP)

    # cleanup
    os.remove(in_file)

    # check these constants are defined
    print(ot.WrapperDataVariableType.IN, ot.WrapperDataVariableType.OUT)
    print(ot.WrapperDataFileType.IN, ot.WrapperDataFileType.OUT)

except:
    import sys
    import traceback
    traceback.print_exc()
