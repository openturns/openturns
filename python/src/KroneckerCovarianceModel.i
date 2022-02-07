// SWIG file KroneckerCovarianceModel.i

%{
#include "openturns/KroneckerCovarianceModel.hxx"
%}

%include KroneckerCovarianceModel_doc.i

%include openturns/KroneckerCovarianceModel.hxx

namespace OT { %extend KroneckerCovarianceModel { KroneckerCovarianceModel(const KroneckerCovarianceModel & other) { return new OT::KroneckerCovarianceModel(other); } } }
