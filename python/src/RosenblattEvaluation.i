// SWIG file RosenblattEvaluation.i

%{
#include "RosenblattEvaluation.hxx"
%}

%include RosenblattEvaluation.hxx
namespace OT { %extend RosenblattEvaluation { RosenblattEvaluation(const RosenblattEvaluation & other) { return new OT::RosenblattEvaluation(other); } } }
