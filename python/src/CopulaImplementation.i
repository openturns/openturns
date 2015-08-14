// SWIG file CopulaImplementation.i

%{
#include "CopulaImplementation.hxx"
%}

%include CopulaImplementation_doc.i

%include CopulaImplementation.hxx
namespace OT { %extend CopulaImplementation { CopulaImplementation(const CopulaImplementation & other) { return new OT::CopulaImplementation(other); } } }
