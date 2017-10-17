// SWIG file NormInfEnumerateFunction.i

%{
#include "openturns/NormInfEnumerateFunction.hxx"
%}

%include NormInfEnumerateFunction_doc.i

%include openturns/NormInfEnumerateFunction.hxx
namespace OT { %extend NormInfEnumerateFunction { NormInfEnumerateFunction(const NormInfEnumerateFunction & other) { return new OT::NormInfEnumerateFunction(other); } } }
