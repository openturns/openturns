// SWIG file UserDefinedCovarianceModel.i

%{
#include "UserDefinedCovarianceModel.hxx"
%}

%include UserDefinedCovarianceModel_doc.i

%include UserDefinedCovarianceModel.hxx

namespace OT{ %extend UserDefinedCovarianceModel { UserDefinedCovarianceModel(const UserDefinedCovarianceModel & other) { return new OT::UserDefinedCovarianceModel(other); } } }
