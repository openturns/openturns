// SWIG file IsotropicCovarianceModel.i

%{
#include "openturns/IsotropicCovarianceModel.hxx"
%}

%include IsotropicCovarianceModel_doc.i

%include openturns/IsotropicCovarianceModel.hxx
namespace OT { %extend IsotropicCovarianceModel { IsotropicCovarianceModel(const IsotropicCovarianceModel & other) { return new OT::IsotropicCovarianceModel(other); } } }
