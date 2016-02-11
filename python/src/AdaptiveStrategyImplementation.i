// SWIG file AdaptiveStrategyImplementation.i

%{
#include "openturns/AdaptiveStrategyImplementation.hxx"
%}

%include AdaptiveStrategyImplementation_doc.i

%include openturns/AdaptiveStrategyImplementation.hxx
namespace OT{ %extend AdaptiveStrategyImplementation { AdaptiveStrategyImplementation(const AdaptiveStrategyImplementation & other) { return new OT::AdaptiveStrategyImplementation(other); } } }
