// SWIG file Gradient.i

%{
#include "openturns/Gradient.hxx"
%}

%include Gradient_doc.i

OTTypedInterfaceObjectHelper(Gradient)

%include openturns/Gradient.hxx
namespace OT{ %extend Gradient { Gradient(const Gradient & other) { return new OT::Gradient(other); } } }
