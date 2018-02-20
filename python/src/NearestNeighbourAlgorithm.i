// SWIG file NearestNeighbourAlgorithm.i

%{
#include "openturns/NearestNeighbourAlgorithm.hxx"
%}

%include NearestNeighbourAlgorithm_doc.i

OTTypedInterfaceObjectHelper(NearestNeighbourAlgorithm)

%include openturns/NearestNeighbourAlgorithm.hxx
namespace OT{ %extend NearestNeighbourAlgorithm { NearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & other) { return new OT::NearestNeighbourAlgorithm(other); } } }
