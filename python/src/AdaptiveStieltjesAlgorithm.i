// SWIG file AdaptiveStieltjesAlgorithm.i

%{
#include "AdaptiveStieltjesAlgorithm.hxx"
%}

%include AdaptiveStieltjesAlgorithm_doc.i

%include AdaptiveStieltjesAlgorithm.hxx
namespace OT{ %extend AdaptiveStieltjesAlgorithm { AdaptiveStieltjesAlgorithm(const AdaptiveStieltjesAlgorithm & other) { return new OT::AdaptiveStieltjesAlgorithm(other); } } }
