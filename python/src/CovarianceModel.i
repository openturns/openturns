// SWIG file CovarianceModel.i

%{
#include "openturns/CovarianceModel.hxx"
%}

%include CovarianceModel_doc.i

OTTypedInterfaceObjectHelper(CovarianceModel)

%include openturns/CovarianceModel.hxx
namespace OT{ %extend CovarianceModel { CovarianceModel(const CovarianceModel & other) { return new OT::CovarianceModel(other); } } }
