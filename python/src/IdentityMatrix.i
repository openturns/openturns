// SWIG file IdentityMatrix.i

%{
#include "IdentityMatrix.hxx"
%}

%include IdentityMatrix.hxx

namespace OT {  

%extend IdentityMatrix {
  IdentityMatrix(const IdentityMatrix & other) { return new OT::IdentityMatrix(other); }

  void __setitem__(PyObject * args, NumericalScalar val) {

    SWIG_exception_fail(SWIG_RuntimeError, "method IdentityMatrix___setitem__ is not available because these matrices are read-only");
fail:
    return;
  }

} // IdentityMatrix
} // OT
