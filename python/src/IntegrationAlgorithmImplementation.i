// SWIG file IntegrationAlgorithmImplementation.i

%{
#include "IntegrationAlgorithmImplementation.hxx"
%}

%include IntegrationAlgorithmImplementation.hxx
namespace OT { %extend IntegrationAlgorithmImplementation { IntegrationAlgorithmImplementation(const IntegrationAlgorithmImplementation & other) { return new OT::IntegrationAlgorithmImplementation(other); } } }
