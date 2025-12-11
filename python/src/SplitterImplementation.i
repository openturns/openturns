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
  OT::Indices indicesTrain;
  OT::Indices indicesTest;
  try {
    indicesTrain = self->generate(indicesTest);
  }
  catch (const OT::OutOfBoundException &) {
    SWIG_SetErrorObj(PyExc_StopIteration, SWIG_Py_Void());
    return 0;
  }
  PyObject* result = PyTuple_New(2);
  PyTuple_SET_ITEM(result, 0, SWIG_NewPointerObj(indicesTrain.clone(), SWIG_TypeQuery("OT::Indices *"), SWIG_POINTER_OWN));
  PyTuple_SET_ITEM(result, 1, SWIG_NewPointerObj(indicesTest.clone(), SWIG_TypeQuery("OT::Indices *"), SWIG_POINTER_OWN));
  return result;
}

} }
