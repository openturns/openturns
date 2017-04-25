// SWIG file PointToPointEvaluation.i

%{
#include "openturns/PointToPointEvaluation.hxx"
%}

%include PointToPointEvaluation_doc.i

%include openturns/PointToPointEvaluation.hxx
namespace OT { %extend PointToPointEvaluation { PointToPointEvaluation(const PointToPointEvaluation & other) { return new OT::PointToPointEvaluation(other); } } }
