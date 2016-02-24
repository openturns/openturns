// SWIG file UniVariateFunctionImplementation.i

%{
#include "openturns/UniVariateFunctionImplementation.hxx"
%}

%include UniVariateFunctionImplementation_doc.i

%include openturns/UniVariateFunctionImplementation.hxx
namespace OT { %extend UniVariateFunctionImplementation { UniVariateFunctionImplementation(const UniVariateFunctionImplementation & other) { return new OT::UniVariateFunctionImplementation(other); } } }
