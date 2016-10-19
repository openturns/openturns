// SWIG file SVDMethod.i

%{
#include "openturns/SVDMethod.hxx"
%}

%include SVDMethod_doc.i

%include openturns/SVDMethod.hxx
namespace OT { %extend SVDMethod { SVDMethod(const SVDMethod & other) { return new OT::SVDMethod(other); } } }
