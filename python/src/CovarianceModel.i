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

namespace OT {

%extend CovarianceModel {

CovarianceModel __rmul__(const Scalar scalar)
{
  return *self * scalar;
}

} // %extend CovarianceModel

} // namespace OT
