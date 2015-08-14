// SWIG file ApproximationAlgorithm.i

%{
#include "ApproximationAlgorithm.hxx"
%}

%include ApproximationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(ApproximationAlgorithm)

%include ApproximationAlgorithm.hxx
namespace OT { %extend ApproximationAlgorithm { ApproximationAlgorithm(const ApproximationAlgorithm & other) { return new OT::ApproximationAlgorithm(other); } } }
