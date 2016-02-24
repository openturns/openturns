// SWIG file RosenblattEvaluation.i

OTAutodoc(RosenblattEvaluation)

%{
#include "openturns/RosenblattEvaluation.hxx"
%}

%include openturns/RosenblattEvaluation.hxx
namespace OT { %extend RosenblattEvaluation { RosenblattEvaluation(const RosenblattEvaluation & other) { return new OT::RosenblattEvaluation(other); } } }
