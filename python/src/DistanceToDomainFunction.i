// SWIG file DistanceToDomainFunction.i

%{
#include "openturns/DistanceToDomainFunction.hxx"
%}

%include DistanceToDomainFunction_doc.i

%include openturns/DistanceToDomainFunction.hxx
namespace OT { %extend DistanceToDomainFunction { DistanceToDomainFunction(const DistanceToDomainFunction & other) { return new OT::DistanceToDomainFunction(other); } } }
