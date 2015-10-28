// SWIG file TenorizedCovarianceModel.i

%{
#include "TensorizedCovarianceModel.hxx"
%}

%include TensorizedCovarianceModel_doc.i

%template(CovarianceModelCollection)           OT::Collection<OT::CovarianceModel>;
%template(CovarianceModelPersistentCollection) OT::PersistentCollection<OT::CovarianceModel>;

%include TensorizedCovarianceModel.hxx
namespace OT{ %extend TensorizedCovarianceModel { TensorizedCovarianceModel(const TensorizedCovarianceModel & other) { return new OT::TensorizedCovarianceModel(other); } } }
