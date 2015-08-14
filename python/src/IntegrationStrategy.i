// SWIG file IntegrationStrategy.i

%{
#include "IntegrationStrategy.hxx"
%}

%include IntegrationStrategy_doc.i

%include IntegrationStrategy.hxx
namespace OT{ %extend IntegrationStrategy { IntegrationStrategy(const IntegrationStrategy & other) { return new OT::IntegrationStrategy(other); } } }
