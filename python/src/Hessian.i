// SWIG file Hessian.i

%{
#include "openturns/Hessian.hxx"
%}

%include Hessian_doc.i

OTTypedInterfaceObjectHelper(Hessian)

%copyctor OT::Hessian;

%include openturns/Hessian.hxx
