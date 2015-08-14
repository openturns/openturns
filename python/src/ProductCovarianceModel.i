// SWIG file ProductCovarianceModel.i

%{
#include "ProductCovarianceModel.hxx"
%}

%include ProductCovarianceModel_doc.i

%template(CovarianceModelCollection)           OT::Collection<OT::CovarianceModel>;
%template(CovarianceModelPersistentCollection) OT::PersistentCollection<OT::CovarianceModel>;

%include ProductCovarianceModel.hxx
namespace OT{ %extend ProductCovarianceModel { ProductCovarianceModel(const ProductCovarianceModel & other) { return new OT::ProductCovarianceModel(other); } } }
