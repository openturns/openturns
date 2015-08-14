// SWIG file InverseNormal.i

%{
#include "InverseNormal.hxx"
%}

%include InverseNormal_doc.i

%include InverseNormal.hxx
namespace OT { %extend InverseNormal { InverseNormal(const InverseNormal & other) { return new OT::InverseNormal(other); } } }
