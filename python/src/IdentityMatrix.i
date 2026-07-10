// SWIG file IdentityMatrix.i

%{
#include "openturns/IdentityMatrix.hxx"
%}

%include IdentityMatrix_doc.i

%copyctor OT::IdentityMatrix;
%include openturns/IdentityMatrix.hxx

namespace OT {  

%extend IdentityMatrix {

static PyObject * __setitem__(PyObject * , Scalar )
{
  SWIG_exception_fail(SWIG_RuntimeError, "method IdentityMatrix___setitem__ is not available because these matrices are read-only");
  return SWIG_Py_Void();
fail:
  return NULL;
}

} // IdentityMatrix
} // OT
