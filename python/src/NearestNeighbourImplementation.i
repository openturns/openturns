// SWIG file NearestNeighbourImplementation.i

%{
#include "openturns/NearestNeighbourImplementation.hxx"
%}

%include NearestNeighbourImplementation_doc.i

%include openturns/NearestNeighbourImplementation.hxx
namespace OT{ %extend NearestNeighbourImplementation { NearestNeighbourImplementation(const NearestNeighbourImplementation & other) { return new OT::NearestNeighbourImplementation(other); } } }
