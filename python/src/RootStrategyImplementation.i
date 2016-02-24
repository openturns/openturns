// SWIG file RootStrategyImplementation.i

%{
#include "openturns/RootStrategyImplementation.hxx"
%}

%include RootStrategyImplementation_doc.i

%include openturns/RootStrategyImplementation.hxx
namespace OT{ %extend RootStrategyImplementation { RootStrategyImplementation(const RootStrategyImplementation & other) { return new OT::RootStrategyImplementation(other); } } }
