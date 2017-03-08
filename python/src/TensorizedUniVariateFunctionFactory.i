// SWIG file TensorizedUniVariateFunctionFactory.i

%{
#include "openturns/TensorizedUniVariateFunctionFactory.hxx"
%}

%include TensorizedUniVariateFunctionFactory_doc.i

%include openturns/TensorizedUniVariateFunctionFactory.hxx
namespace OT{ %extend TensorizedUniVariateFunctionFactory { TensorizedUniVariateFunctionFactory(const TensorizedUniVariateFunctionFactory & other) { return new OT::TensorizedUniVariateFunctionFactory(other); } } }

