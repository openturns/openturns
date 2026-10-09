// SWIG file SumCovarianceModel.i

%{
#include "openturns/SumCovarianceModel.hxx"
%}

%include SumCovarianceModel_doc.i

// Collection converters for CovarianceModel sequences live in the
// statistics module; the experimental module needs its own copies
// to accept Python sequences in the collection constructor.
OTDefaultCollectionConvertFunctions(CovarianceModel)

%copyctor OT::SumCovarianceModel;

%include openturns/SumCovarianceModel.hxx
