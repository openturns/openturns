// SWIG file HyperbolicAnisotropicEnumerateFunction.i

%{
#include "HyperbolicAnisotropicEnumerateFunction.hxx"
%}

%include HyperbolicAnisotropicEnumerateFunction_doc.i

%include HyperbolicAnisotropicEnumerateFunction.hxx
namespace OT { %extend HyperbolicAnisotropicEnumerateFunction { HyperbolicAnisotropicEnumerateFunction(const HyperbolicAnisotropicEnumerateFunction & other) { return new OT::HyperbolicAnisotropicEnumerateFunction(other); } } }
