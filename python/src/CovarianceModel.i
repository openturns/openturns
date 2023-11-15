// SWIG file CovarianceModel.i

%{
#include "openturns/CovarianceModel.hxx"
%}

%include CovarianceModel_doc.i

OTDefaultCollectionConvertFunctions(CovarianceModel)
OTTypedInterfaceObjectHelper(CovarianceModel)
OTTypedCollectionInterfaceObjectHelper(CovarianceModel)

%copyctor OT::CovarianceModel;

%include openturns/CovarianceModel.hxx
