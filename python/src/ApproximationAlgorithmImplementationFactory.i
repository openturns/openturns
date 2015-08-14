// SWIG file ApproximationAlgorithmImplementationFactory.i

%{
#include "ApproximationAlgorithmImplementationFactory.hxx"
%}

%include ApproximationAlgorithmImplementationFactory_doc.i

%include ApproximationAlgorithmImplementationFactory.hxx
namespace OT { %extend ApproximationAlgorithmImplementationFactory { ApproximationAlgorithmImplementationFactory(const ApproximationAlgorithmImplementationFactory & other) { return new OT::ApproximationAlgorithmImplementationFactory(other); } } }
