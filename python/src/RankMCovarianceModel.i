// SWIG file RankMCovarianceModel.i

%{
#include "openturns/RankMCovarianceModel.hxx"
%}

%include RankMCovarianceModel_doc.i

%include openturns/RankMCovarianceModel.hxx
namespace OT{ %extend RankMCovarianceModel { RankMCovarianceModel(const RankMCovarianceModel & other) { return new OT::RankMCovarianceModel(other); } } }
