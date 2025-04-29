// SWIG file DeconditionedRandomVector.i

%{
#include "openturns/DeconditionedRandomVector.hxx"
%}

%include DeconditionedRandomVector_doc.i

%copyctor OT::DeconditionedRandomVector;

%include openturns/DeconditionedRandomVector.hxx

%pythoncode %{
def ConditionalRandomVector(distribution, randomParameters):
    """
    Retro-compatibility alias for DeconditionedRandomVector.
    """
    openturns.common.Log.Warn('class ConditionalRandomVector is deprecated in favor of DeconditionedRandomVector')
    return openturns.metamodel.DeconditionedRandomVector(distribution, randomParameters)
%}

