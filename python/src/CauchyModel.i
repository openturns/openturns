// SWIG file CauchyModel.i

%{
#include "openturns/CauchyModel.hxx"
%}

%include CauchyModel_doc.i

%include openturns/CauchyModel.hxx
namespace OT { %extend CauchyModel { CauchyModel(const CauchyModel & other) { return new OT::CauchyModel(other); } } }
