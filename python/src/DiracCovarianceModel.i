// SWIG file DiracCovarianceModel.i

%{
#include "DiracCovarianceModel.hxx"
%}

%include DiracCovarianceModel_doc.i

%include DiracCovarianceModel.hxx
namespace OT{ %extend DiracCovarianceModel { DiracCovarianceModel(const DiracCovarianceModel & other) { return new OT::DiracCovarianceModel(other); } } }
