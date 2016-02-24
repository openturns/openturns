// SWIG file OrthogonalFunctionFactory.i

%{
#include "openturns/OrthogonalFunctionFactory.hxx"
%}

%include OrthogonalFunctionFactory_doc.i

%include openturns/OrthogonalFunctionFactory.hxx
namespace OT{ %extend OrthogonalFunctionFactory { OrthogonalFunctionFactory(const OrthogonalFunctionFactory & other) { return new OT::OrthogonalFunctionFactory(other); } } }
