// SWIG file ApproximationAlgorithmImplementation.i

%{
#include "ApproximationAlgorithmImplementation.hxx"
%}

%include ApproximationAlgorithmImplementation_doc.i

%include ApproximationAlgorithmImplementation.hxx
namespace OT { %extend ApproximationAlgorithmImplementation { ApproximationAlgorithmImplementation(const ApproximationAlgorithmImplementation & other) { return new OT::ApproximationAlgorithmImplementation(other); } } }
