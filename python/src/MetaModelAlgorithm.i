// SWIG file MetaModelAlgorithm.i

%{
#include "openturns/MetaModelAlgorithm.hxx"
%}

%include MetaModelAlgorithm_doc.i

%include openturns/MetaModelAlgorithm.hxx
namespace OT{ %extend MetaModelAlgorithm { MetaModelAlgorithm(const MetaModelAlgorithm & other) { return new OT::MetaModelAlgorithm(other); } } }
