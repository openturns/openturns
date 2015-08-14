// SWIG file MetaModelAlgorithm.i

%{
#include "MetaModelAlgorithm.hxx"
%}

%include MetaModelAlgorithm_doc.i

%include MetaModelAlgorithm.hxx
namespace OT{ %extend MetaModelAlgorithm { MetaModelAlgorithm(const MetaModelAlgorithm & other) { return new OT::MetaModelAlgorithm(other); } } }
