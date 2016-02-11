// SWIG file UserDefinedCovarianceModel.i

%{
#include "openturns/UserDefinedCovarianceModel.hxx"
%}

%include UserDefinedCovarianceModel_doc.i

%include openturns/UserDefinedCovarianceModel.hxx

namespace OT{ %extend UserDefinedCovarianceModel { UserDefinedCovarianceModel(const UserDefinedCovarianceModel & other) { return new OT::UserDefinedCovarianceModel(other); } } }
