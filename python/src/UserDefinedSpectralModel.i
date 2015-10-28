// SWIG file UserDefinedSpectralModel.i

%{
#include "UserDefinedSpectralModel.hxx"
%}

%include UserDefinedSpectralModel_doc.i

%include UserDefinedSpectralModel.hxx
namespace OT{ %extend UserDefinedSpectralModel { UserDefinedSpectralModel(const UserDefinedSpectralModel & other) { return new OT::UserDefinedSpectralModel(other); } } }
