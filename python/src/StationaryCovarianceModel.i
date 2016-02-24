// SWIG file StationaryCovarianceModel.i

OTAutodoc(StationaryCovarianceModel)

%{
#include "openturns/StationaryCovarianceModel.hxx"
%}


%include openturns/StationaryCovarianceModel.hxx
namespace OT{ %extend StationaryCovarianceModel { StationaryCovarianceModel(const StationaryCovarianceModel & other) { return new OT::StationaryCovarianceModel(other); } } }
