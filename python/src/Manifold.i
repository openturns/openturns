// SWIG file Manifold.i

%{
#include "openturns/Manifold.hxx"
%}

%include Manifold_doc.i

OTTypedInterfaceObjectHelper(Manifold)

%copyctor OT::Manifold;

%include openturns/Manifold.hxx
