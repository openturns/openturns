// SWIG file DistanceToDomainEvaluation.i

OTAutodoc(DistanceToDomainEvaluation)

%{
#include "openturns/DistanceToDomainEvaluation.hxx"
%}

%include openturns/DistanceToDomainEvaluation.hxx
namespace OT { %extend DistanceToDomainEvaluation { DistanceToDomainEvaluation(const DistanceToDomainEvaluation & other) { return new OT::DistanceToDomainEvaluation(other); } } }
