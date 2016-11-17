// SWIG file IntegrationAlgorithmImplementation.i

%{
#include "openturns/IntegrationAlgorithmImplementation.hxx"
%}

%include IntegrationAlgorithmImplementation_doc.i

%include openturns/IntegrationAlgorithmImplementation.hxx
namespace OT { %extend IntegrationAlgorithmImplementation { IntegrationAlgorithmImplementation(const IntegrationAlgorithmImplementation & other) { return new OT::IntegrationAlgorithmImplementation(other); } } }
