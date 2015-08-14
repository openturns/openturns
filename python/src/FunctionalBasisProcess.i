// SWIG file FunctionalBasisProcess.i

%{
#include "FunctionalBasisProcess.hxx"
%}

%include FunctionalBasisProcess.hxx
namespace OT { %extend FunctionalBasisProcess { FunctionalBasisProcess(const FunctionalBasisProcess & other) { return new OT::FunctionalBasisProcess(other); } } }
