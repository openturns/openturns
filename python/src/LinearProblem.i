// SWIG file LinearProblem.i

%{
#include "openturns/LinearProblem.hxx"
%}

%include LinearProblem_doc.i

%copyctor OT::LinearProblem;

%include openturns/LinearProblem.hxx

namespace OT {

%extend LinearProblem {

  // the method is hidden from SWIG in the class declaration, as SparseMatrix is
  // only exposed by the experimental module
  OT::SparseMatrix getSparseLinearConstraintCoefficients() const { return self->getSparseLinearConstraintCoefficients(); }

} // LinearProblem
} // OT
