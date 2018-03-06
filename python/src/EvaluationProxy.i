// SWIG file EvaluationProxy.i

%{
#include "openturns/EvaluationProxy.hxx"
%}

%include openturns/EvaluationProxy.hxx
namespace OT{ %extend EvaluationProxy { EvaluationProxy(const EvaluationProxy & other) { return new OT::EvaluationProxy(other); } } }
