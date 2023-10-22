// SWIG file IntegrationStrategy.i

%{
#include "openturns/IntegrationStrategy.hxx"
%}

%include IntegrationStrategy_doc.i

%include openturns/IntegrationStrategy.hxx
namespace OT{ %extend IntegrationStrategy { IntegrationStrategy(const IntegrationStrategy & other) { return new OT::IntegrationStrategy(other); } } }
