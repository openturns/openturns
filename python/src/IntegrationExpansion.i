// SWIG file IntegrationExpansion.i

%{
#include "openturns/IntegrationExpansion.hxx"
%}

%include IntegrationExpansion_doc.i

%include openturns/IntegrationExpansion.hxx
namespace OT { %extend IntegrationExpansion { IntegrationExpansion(const IntegrationExpansion & other) { return new OT::IntegrationExpansion(other); } } }
