// SWIG file LatentVariableModel.i

%{
#include "openturns/LatentVariableModel.hxx"
%}

%include LatentVariableModel_doc.i

%include openturns/LatentVariableModel.hxx
namespace OT { %extend LatentVariableModel { LatentVariableModel(const LatentVariableModel & other) { return new OT::LatentVariableModel(other); } } }
