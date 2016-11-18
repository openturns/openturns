// SWIG file CholeskyMethod.i

%{
#include "openturns/CholeskyMethod.hxx"
%}

%include CholeskyMethod_doc.i

%include openturns/CholeskyMethod.hxx
namespace OT { %extend CholeskyMethod { CholeskyMethod(const CholeskyMethod & other) { return new OT::CholeskyMethod(other); } } }
