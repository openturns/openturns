// SWIG file ApproximationAlgorithmImplementationFactory.i

%{
#include "openturns/ApproximationAlgorithmImplementationFactory.hxx"
%}

%include ApproximationAlgorithmImplementationFactory_doc.i

%include openturns/ApproximationAlgorithmImplementationFactory.hxx
namespace OT { %extend ApproximationAlgorithmImplementationFactory { ApproximationAlgorithmImplementationFactory(const ApproximationAlgorithmImplementationFactory & other) { return new OT::ApproximationAlgorithmImplementationFactory(other); } } }
