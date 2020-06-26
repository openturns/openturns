// SWIG file StationaryFunctionalCovarianceModel.i

%{
#include "openturns/StationaryFunctionalCovarianceModel.hxx"
%}

%include StationaryFunctionalCovarianceModel_doc.i

%include openturns/StationaryFunctionalCovarianceModel.hxx
namespace OT { %extend StationaryFunctionalCovarianceModel { StationaryFunctionalCovarianceModel(const StationaryFunctionalCovarianceModel & other) { return new OT::StationaryFunctionalCovarianceModel(other); } } }
