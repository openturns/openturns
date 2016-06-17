// SWIG file TenorizedCovarianceModel.i

%{
#include "openturns/TensorizedCovarianceModel.hxx"
%}

%include TensorizedCovarianceModel_doc.i

%include openturns/TensorizedCovarianceModel.hxx
namespace OT{ %extend TensorizedCovarianceModel { TensorizedCovarianceModel(const TensorizedCovarianceModel & other) { return new OT::TensorizedCovarianceModel(other); } } }
