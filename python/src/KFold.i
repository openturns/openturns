// SWIG file KFold.i

%{
#include "KFold.hxx"
%}

%include KFold_doc.i

%include KFold.hxx
namespace OT { %extend KFold { KFold(const KFold & other) { return new OT::KFold(other); } } }
