// SWIG file GowerDistanceModel.i

%{
#include "openturns/GowerDistanceModel.hxx"
%}

%include GowerDistanceModel_doc.i

%include openturns/GowerDistanceModel.hxx
namespace OT { %extend GowerDistanceModel { GowerDistanceModel(const GowerDistanceModel & other) { return new OT::GowerDistanceModel(other); } } }
