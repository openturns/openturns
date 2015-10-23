// SWIG file StationaryCovarianceModel.i

%{
#include "StationaryCovarianceModel.hxx"
%}


%include StationaryCovarianceModel.hxx
namespace OT{ %extend StationaryCovarianceModel { StationaryCovarianceModel(const StationaryCovarianceModel & other) { return new OT::StationaryCovarianceModel(other); } } }
