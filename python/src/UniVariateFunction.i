// SWIG file UniVariateFunction.i

%{
#include "openturns/UniVariateFunction.hxx"
%}

%include UniVariateFunction_doc.i

OTTypedInterfaceObjectHelper(UniVariateFunction)

%include openturns/UniVariateFunction.hxx
namespace OT {

%extend UniVariateFunction {

UniVariateFunction(const UniVariateFunction & other) { return new OT::UniVariateFunction(other); }

}

}
