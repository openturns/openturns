// SWIG file QuadraticProgrammingProblem.i

%{
#include "openturns/QuadraticProgrammingProblem.hxx"
%}

%include QuadraticProgrammingProblem_doc.i

%copyctor OT::QuadraticProgrammingProblem;

%include openturns/QuadraticProgrammingProblem.hxx

%pythoncode %{
def QuadraticProblem(*args):
    """Deprecated"""
    openturns.common.Log.Warn('class QuadraticProblem is deprecated in favor of QuadraticProgrammingProblem')
    return openturns.experimental.QuadraticProgrammingProblem(*args)
%}
