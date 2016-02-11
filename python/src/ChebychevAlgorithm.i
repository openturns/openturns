// SWIG file ChebychevAlgorithm.i

%{
#include "openturns/ChebychevAlgorithm.hxx"
%}

%include ChebychevAlgorithm_doc.i

%include openturns/ChebychevAlgorithm.hxx
namespace OT{ %extend ChebychevAlgorithm { ChebychevAlgorithm(const ChebychevAlgorithm & other) { return new OT::ChebychevAlgorithm(other); } } }
