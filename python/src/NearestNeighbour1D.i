// SWIG file NearestNeighbour1D.i

%{
#include "openturns/NearestNeighbour1D.hxx"
%}

%include NearestNeighbour1D_doc.i

%include openturns/NearestNeighbour1D.hxx
namespace OT { %extend NearestNeighbour1D { NearestNeighbour1D(const NearestNeighbour1D & other) { return new OT::NearestNeighbour1D(other); } } }
