// SWIG file ProductCovarianceModel.i

%{
#include "openturns/ProductCovarianceModel.hxx"
%}

%include ProductCovarianceModel_doc.i

%template(CovarianceModelCollection)           OT::Collection<OT::CovarianceModel>;
%template(CovarianceModelPersistentCollection) OT::PersistentCollection<OT::CovarianceModel>;

%include openturns/ProductCovarianceModel.hxx
namespace OT{ %extend ProductCovarianceModel { ProductCovarianceModel(const ProductCovarianceModel & other) { return new OT::ProductCovarianceModel(other); } } }
