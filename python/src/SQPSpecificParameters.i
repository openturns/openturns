// SWIG file SQPSpecificParameters.i

%{
#include "SQPSpecificParameters.hxx"
%}

%include SQPSpecificParameters.hxx
namespace OT{ %extend SQPSpecificParameters { SQPSpecificParameters(const SQPSpecificParameters & other) { return new OT::SQPSpecificParameters(other); } } }
