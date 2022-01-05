// SWIG file Object.i

%ignore OT::Object::GetClassName;

%{
#include "openturns/Object.hxx"
%}

%include Object_doc.i

%include openturns/Object.hxx
namespace OT{ %extend Object { Object(const Object & other) { return new OT::Object(other); } } }

%pythoncode %{
def Object___getstate__(self):
    import tempfile
    import os
    state = dict()
    study = Study()

    # assume xml support
    infile = tempfile.NamedTemporaryFile(delete=False, suffix='.xml')
    try:
        study.setStorageManager(XMLH5StorageManager(infile.name))
        state['format'] = 'xmlh5'
    except:
        study.setStorageManager(XMLStorageManager(infile.name))
        state['format'] = 'xml'
    study.add('instance', self)
    study.save()
    infile.seek(0)
    state['xmldata'] = infile.read()
    infile.close()
    os.remove(infile.name)
    if state['format'] == 'xmlh5':
        h5filename = infile.name.replace('.xml', '.h5')
        try:
            with open(h5filename, 'rb') as h5file:
                state['h5data'] = h5file.read()
            os.remove(h5filename)
        except FileNotFoundError:
            pass # no h5 data
    return state

Object.__getstate__ = Object___getstate__

def Object___setstate__(self, state):
    import tempfile
    import os

    # call ctor to initialize underlying cxx obj
    # as it is instanciated from object.__new__
    self.__init__()

    study = Study()
    outfile = tempfile.NamedTemporaryFile(delete=False, suffix='.xml')
    outfile.write(state['xmldata'])
    outfile.seek(0)
    if 'h5data' in state:
        h5filename = outfile.name.replace('.xml', '.h5')
        with open(h5filename, 'wb') as h5file:
            h5file.write(state['h5data'])
    if state.get('format', 'xml') == 'xmlh5':
        study.setStorageManager(XMLH5StorageManager(outfile.name))
    else:
        study.setStorageManager(XMLStorageManager(outfile.name))
    study.load()
    outfile.close()
    os.remove(outfile.name)
    if 'h5data' in state:
        os.remove(h5filename)
    study.fillObject('instance', self)

Object.__setstate__ = Object___setstate__

def Object__repr_html_(self):
    lines = str(self).splitlines()
    return '<p>' + '<br>\n'.join(lines) + '</p>'
Object._repr_html_ = Object__repr_html_
%}
