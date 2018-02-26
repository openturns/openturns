// SWIG file NearestNeighbourAlgorithmImplementation.i

%{
#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
%}

%include NearestNeighbourAlgorithmImplementation_doc.i

%include openturns/NearestNeighbourAlgorithmImplementation.hxx
namespace OT{ %extend NearestNeighbourAlgorithmImplementation { NearestNeighbourAlgorithmImplementation(const NearestNeighbourAlgorithmImplementation & other) { return new OT::NearestNeighbourAlgorithmImplementation(other); } } }
