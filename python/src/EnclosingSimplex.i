// SWIG file EnclosingSimplex.i

%{
#include "openturns/EnclosingSimplex.hxx"
%}

%include EnclosingSimplex_doc.i

OTTypedInterfaceObjectHelper(EnclosingSimplex)

%include openturns/EnclosingSimplex.hxx
namespace OT{ %extend EnclosingSimplex { EnclosingSimplex(const EnclosingSimplex & other) { return new OT::EnclosingSimplex(other); } } }
