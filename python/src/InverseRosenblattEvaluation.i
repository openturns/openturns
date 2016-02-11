// SWIG file InverseRosenblattEvaluation.i

OTAutodoc(InverseRosenblattEvaluation)

%{
#include "openturns/InverseRosenblattEvaluation.hxx"
%}

%include openturns/InverseRosenblattEvaluation.hxx
namespace OT { %extend InverseRosenblattEvaluation { InverseRosenblattEvaluation(const InverseRosenblattEvaluation & other) { return new OT::InverseRosenblattEvaluation(other); } } }
