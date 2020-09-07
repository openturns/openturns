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
    import os
    state = dict()
    study = Study()

    # assume xml support
    # should use BinaryStorageManager
    infile = tempfile.NamedTemporaryFile(delete=False)
    study.setStorageManager(XMLStorageManager(infile.name))
    study.add('instance', self)
    study.save()
    infile.seek(0)
    state['xmldata'] = infile.read()
    infile.close()
    os.remove(infile.name)

    return state

Object.__getstate__ = Object___getstate__

def Object___setstate__(self, state):
    import tempfile
    import os

    # call ctor to initialize underlying cxx obj
    # as it is instanciated from object.__new__
    self.__init__()

    study = Study()
    outfile = tempfile.NamedTemporaryFile(delete=False)
    outfile.write(state['xmldata'])
    outfile.seek(0)
    study.setStorageManager(XMLStorageManager(outfile.name))
    study.load()
    outfile.close()
    os.remove(outfile.name)

    study.fillObject('instance', self)

Object.__setstate__ = Object___setstate__

def Object__repr_html_(self):
    lines = str(self).splitlines()
    return '<p>' + '<br>\n'.join(lines) + '</p>'
Object._repr_html_ = Object__repr_html_
%}
