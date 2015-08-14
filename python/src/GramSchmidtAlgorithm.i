// SWIG file GramSchmidtAlgorithm.i

%{
#include "GramSchmidtAlgorithm.hxx"
%}

%include GramSchmidtAlgorithm_doc.i

%include GramSchmidtAlgorithm.hxx
namespace OT{ %extend GramSchmidtAlgorithm { GramSchmidtAlgorithm(const GramSchmidtAlgorithm & other) { return new OT::GramSchmidtAlgorithm(other); } } }
