// SWIG file LinearCombinationCovarianceModel.i

%{
#include "openturns/LinearCombinationCovarianceModel.hxx"
%}

%include LinearCombinationCovarianceModel_doc.i

// Collection converters for CovarianceModel sequences live in the
// statistics module; the experimental module needs its own copies
// to accept Python sequences in the collection constructor.
OTDefaultCollectionConvertFunctions(CovarianceModel)

%copyctor OT::LinearCombinationCovarianceModel;

%include openturns/LinearCombinationCovarianceModel.hxx
