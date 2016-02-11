// SWIG file TenorizedCovarianceModel.i

%{
#include "openturns/TensorizedCovarianceModel.hxx"
%}

%include TensorizedCovarianceModel_doc.i

%template(CovarianceModelCollection)           OT::Collection<OT::CovarianceModel>;
%template(CovarianceModelPersistentCollection) OT::PersistentCollection<OT::CovarianceModel>;

%include openturns/TensorizedCovarianceModel.hxx
namespace OT{ %extend TensorizedCovarianceModel { TensorizedCovarianceModel(const TensorizedCovarianceModel & other) { return new OT::TensorizedCovarianceModel(other); } } }
