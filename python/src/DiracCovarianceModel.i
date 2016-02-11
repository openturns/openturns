// SWIG file DiracCovarianceModel.i

%{
#include "openturns/DiracCovarianceModel.hxx"
%}

%include DiracCovarianceModel_doc.i

%include openturns/DiracCovarianceModel.hxx
namespace OT{ %extend DiracCovarianceModel { DiracCovarianceModel(const DiracCovarianceModel & other) { return new OT::DiracCovarianceModel(other); } } }
