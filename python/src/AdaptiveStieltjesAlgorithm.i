// SWIG file AdaptiveStieltjesAlgorithm.i

%{
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"
%}

%include AdaptiveStieltjesAlgorithm_doc.i

%include openturns/AdaptiveStieltjesAlgorithm.hxx
namespace OT{ %extend AdaptiveStieltjesAlgorithm { AdaptiveStieltjesAlgorithm(const AdaptiveStieltjesAlgorithm & other) { return new OT::AdaptiveStieltjesAlgorithm(other); } } }
