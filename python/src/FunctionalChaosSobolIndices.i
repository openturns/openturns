// SWIG file FunctionalChaosSobolIndices.i

%{
#include "openturns/FunctionalChaosSobolIndices.hxx"
%}

%include FunctionalChaosSobolIndices_doc.i

%include openturns/FunctionalChaosSobolIndices.hxx
namespace OT { %extend FunctionalChaosSobolIndices { FunctionalChaosSobolIndices(const FunctionalChaosSobolIndices & other) { return new OT::FunctionalChaosSobolIndices(other); } } }

%pythoncode %{
class FunctionalChaosRandomVector(FunctionalChaosSobolIndices):
    def __init__(self, *args):
        super(FunctionalChaosRandomVector, self).__init__(*args)
        openturns.common.Log.Warn('class FunctionalChaosRandomVector is deprecated in favor of FunctionalChaosSobolIndices')
%}
