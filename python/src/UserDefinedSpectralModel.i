// SWIG file UserDefinedSpectralModel.i

%{
#include "openturns/UserDefinedSpectralModel.hxx"
%}

%include UserDefinedSpectralModel_doc.i

%include openturns/UserDefinedSpectralModel.hxx
namespace OT{ %extend UserDefinedSpectralModel { UserDefinedSpectralModel(const UserDefinedSpectralModel & other) { return new OT::UserDefinedSpectralModel(other); } } }
