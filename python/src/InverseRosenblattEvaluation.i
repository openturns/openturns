// SWIG file InverseRosenblattEvaluation.i

OTAutodoc(InverseRosenblattEvaluation)

%{
#include "InverseRosenblattEvaluation.hxx"
%}

%include InverseRosenblattEvaluation.hxx
namespace OT { %extend InverseRosenblattEvaluation { InverseRosenblattEvaluation(const InverseRosenblattEvaluation & other) { return new OT::InverseRosenblattEvaluation(other); } } }
