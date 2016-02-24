// SWIG file OrthogonalUniVariateFunctionFactory.i

%{
#include "openturns/OrthogonalUniVariateFunctionFactory.hxx"
%}

%include OrthogonalUniVariateFunctionFactory_doc.i

%include openturns/OrthogonalUniVariateFunctionFactory.hxx
namespace OT{ %extend OrthogonalUniVariateFunctionFactory { OrthogonalUniVariateFunctionFactory(const OrthogonalUniVariateFunctionFactory & other) { return new OT::OrthogonalUniVariateFunctionFactory(other); } } }
