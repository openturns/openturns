// SWIG file InverseNormal.i

%{
#include "openturns/InverseNormal.hxx"
%}

%include InverseNormal_doc.i

%include openturns/InverseNormal.hxx
namespace OT { %extend InverseNormal { InverseNormal(const InverseNormal & other) { return new OT::InverseNormal(other); } } }
