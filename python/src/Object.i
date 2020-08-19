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
    import tempfile
    state = dict()
    study = Study()

    # assume xml support
    # should use BinaryStorageManager
    with tempfile.NamedTemporaryFile() as infile:
        study.setStorageManager(XMLStorageManager(infile.name))
        study.add('instance', self)
        study.save()
        infile.seek(0)
        state['xmldata'] = infile.read()

    return state

Object.__getstate__ = Object___getstate__

def Object___setstate__(self, state):
    import tempfile
    # call ctor to initialize underlying cxx obj
    # as it is instanciated from object.__new__
    self.__init__()

    study = Study()
    with tempfile.NamedTemporaryFile() as outfile:
        outfile.write(state['xmldata'])
        outfile.seek(0)
        study.setStorageManager(XMLStorageManager(outfile.name))
        study.load()

    study.fillObject('instance', self)

Object.__setstate__ = Object___setstate__

def Object__repr_html_(self):
    lines = str(self).splitlines()
    return '<p>' + '<br>\n'.join(lines) + '</p>'
Object._repr_html_ = Object__repr_html_
%}
