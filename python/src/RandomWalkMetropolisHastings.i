// SWIG file RandomWalkMetropolisHastings.i

%{
#include "openturns/RandomWalkMetropolisHastings.hxx"
%}

%include RandomWalkMetropolisHastings_doc.i

%include openturns/RandomWalkMetropolisHastings.hxx
namespace OT { %extend RandomWalkMetropolisHastings { RandomWalkMetropolisHastings(const RandomWalkMetropolisHastings & other) { return new OT::RandomWalkMetropolisHastings(other); } } }

%pythoncode %{
class CalibrationStrategy:
    def __init__(self, *args):
        raise TypeError('CalibrationStrategy has been merged into RandomWalkMetropolisHastings')
%}
