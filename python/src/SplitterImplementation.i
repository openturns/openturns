// SWIG file

%{
#include "openturns/SplitterImplementation.hxx"
%}

%include SplitterImplementation_doc.i

%template(_SplitterImplementationdInterfaceObject)           OT::TypedInterfaceObject<OT::SplitterImplementation>;

%ignore OT::SplitterImplementation::reset;
%ignore OT::SplitterImplementation::generate;

%include openturns/SplitterImplementation.hxx

%copyctor OT::SplitterImplementation;

namespace OT { %extend SplitterImplementation {

SplitterImplementation * __iter__()
{
  self->reset();
  return self;
}

PyObject* __next__()
{
  OT::Indices indices1;
  OT::Indices indices2;
  try {
    indices1 = self->generate(indices2);
  }
  catch (const OT::OutOfBoundException &) {
    SWIG_SetErrorObj(PyExc_StopIteration, SWIG_Py_Void());
    return 0;
  }
  PyObject* result = PyTuple_New(2);
  PyTuple_SET_ITEM(result, 0, SWIG_NewPointerObj(indices1.clone(), SWIGTYPE_p_OT__Indices, SWIG_POINTER_OWN | 0));
  PyTuple_SET_ITEM(result, 1, SWIG_NewPointerObj(indices2.clone(), SWIGTYPE_p_OT__Indices, SWIG_POINTER_OWN | 0));
  return result;
}

} }
