// SWIG file RegularGridEnclosingSimplex.i

%{
#include "openturns/RegularGridEnclosingSimplex.hxx"
%}

%include RegularGridEnclosingSimplex_doc.i

%include openturns/RegularGridEnclosingSimplex.hxx
namespace OT { %extend RegularGridEnclosingSimplex { RegularGridEnclosingSimplex(const RegularGridEnclosingSimplex & other) { return new OT::RegularGridEnclosingSimplex(other); } } }
