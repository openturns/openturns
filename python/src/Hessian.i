// SWIG file Hessian.i

%{
#include "openturns/Hessian.hxx"
%}

%include Hessian_doc.i

OTTypedInterfaceObjectHelper(Hessian)

%include openturns/Hessian.hxx
namespace OT{ %extend Hessian { Hessian(const Hessian & other) { return new OT::Hessian(other); } } }
