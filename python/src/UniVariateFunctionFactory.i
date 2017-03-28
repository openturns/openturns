// SWIG file UniVariateFunctionFactory.i

%{
#include "openturns/UniVariateFunctionFactory.hxx"
%}

%include UniVariateFunctionFactory_doc.i

%include openturns/UniVariateFunctionFactory.hxx
namespace OT{ %extend UniVariateFunctionFactory { UniVariateFunctionFactory(const UniVariateFunctionFactory & other) { return new OT::UniVariateFunctionFactory(other); } } }
