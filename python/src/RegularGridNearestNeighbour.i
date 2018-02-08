// SWIG file RegularGridNearestNeighbour.i

%{
#include "openturns/RegularGridNearestNeighbour.hxx"
%}

%include RegularGridNearestNeighbour_doc.i

%include openturns/RegularGridNearestNeighbour.hxx
namespace OT { %extend RegularGridNearestNeighbour { RegularGridNearestNeighbour(const RegularGridNearestNeighbour & other) { return new OT::RegularGridNearestNeighbour(other); } } }
