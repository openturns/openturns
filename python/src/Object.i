// SWIG file Object.i

%ignore OT::Object::GetClassName;

%{
#include "openturns/Object.hxx"
%}

%include Object_doc.i

%include openturns/Object.hxx
namespace OT{ %extend Object { Object(const Object & other) { return new OT::Object(other); } } }

%pythoncode %{
import os

def Object___getstate__(self):

    state = dict()
    study = Study()
    filename = Path.BuildTemporaryFileName('xmlfileXXXXXX')

    # assume xml support
    # should use BinaryStorageManager
    with open(filename, 'rb+') as infile:
        study.setStorageManager(XMLStorageManager(filename)) 
        study.add('instance', self)
        study.save()
        infile.seek(0)
        state['xmldata'] = infile.read()

    os.remove(filename)
    return state

Object.__getstate__ = Object___getstate__

def Object___setstate__(self, state):

    # call ctor to initialize underlying cxx obj
    # as it is instanciated from object.__new__
    self.__init__()

    study = Study()
    filename = Path.BuildTemporaryFileName('xmlfileXXXXXX')
    with open(filename, 'rb+') as outfile:
        outfile.write(state['xmldata'])
        outfile.seek(0)
        study.setStorageManager(XMLStorageManager(filename)) 
        study.load()

    study.fillObject('instance', self)
    os.remove(filename)

Object.__setstate__ = Object___setstate__

def Object__repr_html_(self):
    lines = str(self).splitlines()
    return '<p>' + '<br>\n'.join(lines) + '</p>'
Object._repr_html_ = Object__repr_html_
%}
