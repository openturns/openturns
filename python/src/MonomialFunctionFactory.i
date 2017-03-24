// SWIG file MonomialFunctionFactory.i

%{
#include "openturns/MonomialFunctionFactory.hxx"
%}

%include MonomialFunctionFactory_doc.i

%include openturns/MonomialFunctionFactory.hxx
namespace OT{ %extend MonomialFunctionFactory { MonomialFunctionFactory(const MonomialFunctionFactory & other) { return new OT::MonomialFunctionFactory(other); } } }
