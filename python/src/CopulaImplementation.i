// SWIG file CopulaImplementation.i

%{
#include "openturns/CopulaImplementation.hxx"
%}

%include CopulaImplementation_doc.i

%include openturns/CopulaImplementation.hxx
namespace OT { %extend CopulaImplementation { CopulaImplementation(const CopulaImplementation & other) { return new OT::CopulaImplementation(other); } } }
