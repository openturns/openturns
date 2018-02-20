// SWIG file NaiveNearestNeighbour.i

%{
#include "openturns/NaiveNearestNeighbour.hxx"
%}

%include NaiveNearestNeighbour_doc.i

%include openturns/NaiveNearestNeighbour.hxx
namespace OT { %extend NaiveNearestNeighbour { NaiveNearestNeighbour(const NaiveNearestNeighbour & other) { return new OT::NaiveNearestNeighbour(other); } } }
