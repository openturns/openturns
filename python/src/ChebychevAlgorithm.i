// SWIG file ChebychevAlgorithm.i

%{
#include "ChebychevAlgorithm.hxx"
%}

%include ChebychevAlgorithm_doc.i

%include ChebychevAlgorithm.hxx
namespace OT{ %extend ChebychevAlgorithm { ChebychevAlgorithm(const ChebychevAlgorithm & other) { return new OT::ChebychevAlgorithm(other); } } }
