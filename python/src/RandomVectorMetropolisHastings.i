// SWIG file RandomVectorMetropolisHastings.i

%{
#include "openturns/RandomVectorMetropolisHastings.hxx"
%}

%include RandomVectorMetropolisHastings_doc.i

%include openturns/RandomVectorMetropolisHastings.hxx
namespace OT { %extend RandomVectorMetropolisHastings { RandomVectorMetropolisHastings(const RandomVectorMetropolisHastings & other) { return new OT::RandomVectorMetropolisHastings(other); } } }

%pythoncode %{
class CalibrationStrategy:
    def __init__(self, *args):
        raise TypeError('CalibrationStrategy has been merged into RandomVectorMetropolisHastings')
%}
