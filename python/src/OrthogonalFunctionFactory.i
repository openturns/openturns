// SWIG file OrthogonalFunctionFactory.i

%{
#include "OrthogonalFunctionFactory.hxx"
%}

%include OrthogonalFunctionFactory_doc.i

%include OrthogonalFunctionFactory.hxx
namespace OT{ %extend OrthogonalFunctionFactory { OrthogonalFunctionFactory(const OrthogonalFunctionFactory & other) { return new OT::OrthogonalFunctionFactory(other); } } }
