// SWIG file RankOneTensorEvaluation.i

%{
#include "openturns/RankOneTensorEvaluation.hxx"
%}

//%include RankOneTensorEvaluation_doc.i

%include openturns/RankOneTensorEvaluation.hxx
namespace OT{ %extend RankOneTensorEvaluation { RankOneTensorEvaluation(const RankOneTensorEvaluation & other) { return new OT::RankOneTensorEvaluation(other); } } }
