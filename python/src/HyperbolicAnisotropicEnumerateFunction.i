// SWIG file HyperbolicAnisotropicEnumerateFunction.i

%{
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
%}

%include HyperbolicAnisotropicEnumerateFunction_doc.i

%include openturns/HyperbolicAnisotropicEnumerateFunction.hxx
namespace OT { %extend HyperbolicAnisotropicEnumerateFunction { HyperbolicAnisotropicEnumerateFunction(const HyperbolicAnisotropicEnumerateFunction & other) { return new OT::HyperbolicAnisotropicEnumerateFunction(other); } } }
