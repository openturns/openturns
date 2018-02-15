// SWIG file NearestNeighbour.i

%{
#include "openturns/NearestNeighbour.hxx"
%}

%include NearestNeighbour_doc.i

OTTypedInterfaceObjectHelper(NearestNeighbour)

%include openturns/NearestNeighbour.hxx
namespace OT{ %extend NearestNeighbour { NearestNeighbour(const NearestNeighbour & other) { return new OT::NearestNeighbour(other); } } }
