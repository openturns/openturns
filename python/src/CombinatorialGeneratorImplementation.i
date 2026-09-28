// SWIG file CombinatorialGeneratorImplementation.i

%{
#include "openturns/CombinatorialGeneratorImplementation.hxx"
%}

%include CombinatorialGeneratorImplementation_doc.i

%copyctor OT::CombinatorialGeneratorImplementation;

%include openturns/CombinatorialGeneratorImplementation.hxx

namespace OT { %extend CombinatorialGeneratorImplementation {

// Return an owning clone so that iterating a temporary is safe:
// the iterator lifetime does not depend on the iterated object
%newobject __iter__;
OT::CombinatorialGeneratorImplementation * __iter__()
{
  OT::CombinatorialGeneratorImplementation * p_iter = self->clone();
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
