// SWIG file LinearProgrammingProblem.i

%{
#include "openturns/LinearProgrammingProblem.hxx"
%}

%include LinearProgrammingProblem_doc.i

%copyctor OT::LinearProgrammingProblem;

%include openturns/LinearProgrammingProblem.hxx

namespace OT {

%extend LinearProgrammingProblem {

  // the method is hidden from SWIG in the class declaration, as SparseMatrix is
  // only exposed by the experimental module
  OT::SparseMatrix getSparseLinearConstraintCoefficients() const { return self->getSparseLinearConstraintCoefficients(); }

} // LinearProgrammingProblem
} // OT

%pythoncode %{
def LinearProblem(*args):
    """Deprecated"""
    openturns.common.Log.Warn('class LinearProblem is deprecated in favor of LinearProgrammingProblem')
    return openturns.experimental.LinearProgrammingProblem(*args)
%}
