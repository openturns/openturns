// SWIG file CombinatorialGenerator.i

%{
#include "openturns/CombinatorialGenerator.hxx"
%}

%include CombinatorialGenerator_doc.i

OTTypedInterfaceObjectHelper(CombinatorialGenerator)

%copyctor OT::CombinatorialGenerator;

%include openturns/CombinatorialGenerator.hxx

namespace OT { %extend CombinatorialGenerator {

// Return an owning copy so that iterating a temporary is safe:
// the iterator lifetime does not depend on the iterated object
%newobject __iter__;
OT::CombinatorialGenerator * __iter__()
{
  OT::CombinatorialGenerator * p_iter = new OT::CombinatorialGenerator(*self->getImplementation());
  p_iter->restart();
  return p_iter;
}

PyObject* __next__()
{
  OT::Indices indices;
  try {
    indices = self->generateNext();
  }
  catch (const OT::OutOfBoundException &) {
    SWIG_SetErrorObj(PyExc_StopIteration, SWIG_Py_Void());
    return 0;
  }
  return SWIG_NewPointerObj(indices.clone(), SWIG_TypeQuery("OT::Indices *"), SWIG_POINTER_OWN);
}

} }
