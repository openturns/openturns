// SWIG file OrthogonalProductFunctionFactory.i

%{
#include "openturns/OrthogonalProductFunctionFactory.hxx"
%}

%include OrthogonalProductFunctionFactory_doc.i

%include openturns/OrthogonalProductFunctionFactory.hxx
namespace OT{ %extend OrthogonalProductFunctionFactory { OrthogonalProductFunctionFactory(const OrthogonalProductFunctionFactory & other) { return new OT::OrthogonalProductFunctionFactory(other); } } }

