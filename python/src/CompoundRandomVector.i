// SWIG file CompoundRandomVector.i

%{
#include "openturns/CompoundRandomVector.hxx"
%}

%include CompoundRandomVector_doc.i

%copyctor OT::CompoundRandomVector;

%include openturns/CompoundRandomVector.hxx

%pythoncode %{
def DeconditionedRandomVector(conditionedDist, parametersRandomVector):
    """
    Retro-compatibility alias for CompoundRandomVector.
    """
    openturns.common.Log.Warn('class DeconditionedRandomVector is deprecated in favor of CompoundRandomVector')
    return openturns.metamodel.CompoundRandomVector(conditionedDist, parametersRandomVector)
%}
