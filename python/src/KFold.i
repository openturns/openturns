// SWIG file KFold.i

%{
#include "openturns/KFold.hxx"
%}

%include KFold_doc.i

%include openturns/KFold.hxx
namespace OT { %extend KFold { KFold(const KFold & other) { return new OT::KFold(other); } } }
