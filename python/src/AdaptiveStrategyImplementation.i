// SWIG file AdaptiveStrategyImplementation.i

%{
#include "AdaptiveStrategyImplementation.hxx"
%}

%include AdaptiveStrategyImplementation_doc.i

%include AdaptiveStrategyImplementation.hxx
namespace OT{ %extend AdaptiveStrategyImplementation { AdaptiveStrategyImplementation(const AdaptiveStrategyImplementation & other) { return new OT::AdaptiveStrategyImplementation(other); } } }
