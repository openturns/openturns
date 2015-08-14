// SWIG file RootStrategyImplementation.i

%{
#include "RootStrategyImplementation.hxx"
%}

%include RootStrategyImplementation_doc.i

%include RootStrategyImplementation.hxx
namespace OT{ %extend RootStrategyImplementation { RootStrategyImplementation(const RootStrategyImplementation & other) { return new OT::RootStrategyImplementation(other); } } }
