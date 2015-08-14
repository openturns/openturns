// SWIG file CovarianceModel.i

%{
#include "CovarianceModel.hxx"
%}

%include CovarianceModel_doc.i

OTTypedInterfaceObjectHelper(CovarianceModel)

%include CovarianceModel.hxx
namespace OT{ %extend CovarianceModel { CovarianceModel(const CovarianceModel & other) { return new OT::CovarianceModel(other); } } }
