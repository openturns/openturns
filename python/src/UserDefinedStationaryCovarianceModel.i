// SWIG file UserDefinedStationaryCovarianceModel.i

%{
#include "openturns/UserDefinedStationaryCovarianceModel.hxx"
%}

%include UserDefinedStationaryCovarianceModel_doc.i

%include openturns/UserDefinedStationaryCovarianceModel.hxx
namespace OT{ %extend UserDefinedStationaryCovarianceModel { UserDefinedStationaryCovarianceModel(const UserDefinedStationaryCovarianceModel & other) { return new OT::UserDefinedStationaryCovarianceModel(other); } } }
