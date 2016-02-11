// SWIG file ApproximationAlgorithm.i

%{
#include "openturns/ApproximationAlgorithm.hxx"
%}

%include ApproximationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(ApproximationAlgorithm)

%include openturns/ApproximationAlgorithm.hxx
namespace OT { %extend ApproximationAlgorithm { ApproximationAlgorithm(const ApproximationAlgorithm & other) { return new OT::ApproximationAlgorithm(other); } } }
