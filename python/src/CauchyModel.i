// SWIG file CauchyModel.i

%{
#include "CauchyModel.hxx"
%}

%include CauchyModel_doc.i

%include CauchyModel.hxx
namespace OT { %extend CauchyModel { CauchyModel(const CauchyModel & other) { return new OT::CauchyModel(other); } } }
