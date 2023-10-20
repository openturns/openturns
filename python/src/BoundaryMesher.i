// SWIG file BoundaryMesher.i

%{
#include "openturns/BoundaryMesher.hxx"
%}

%include BoundaryMesher_doc.i

%include openturns/BoundaryMesher.hxx

namespace OT {%extend BoundaryMesher {BoundaryMesher(const BoundaryMesher & other){return new OT::BoundaryMesher(other);}}}
