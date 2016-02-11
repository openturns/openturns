// SWIG file FunctionalBasisProcess.i

%{
#include "openturns/FunctionalBasisProcess.hxx"
%}

%include FunctionalBasisProcess_doc.i

%include openturns/FunctionalBasisProcess.hxx
namespace OT { %extend FunctionalBasisProcess { FunctionalBasisProcess(const FunctionalBasisProcess & other) { return new OT::FunctionalBasisProcess(other); } } }
