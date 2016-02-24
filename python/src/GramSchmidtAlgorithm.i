// SWIG file GramSchmidtAlgorithm.i

%{
#include "openturns/GramSchmidtAlgorithm.hxx"
%}

%include GramSchmidtAlgorithm_doc.i

%include openturns/GramSchmidtAlgorithm.hxx
namespace OT{ %extend GramSchmidtAlgorithm { GramSchmidtAlgorithm(const GramSchmidtAlgorithm & other) { return new OT::GramSchmidtAlgorithm(other); } } }
