// SWIG file TruncatedNormal.i

%{
#include "TruncatedNormal.hxx"
%}

%include TruncatedNormal_doc.i

%include TruncatedNormal.hxx
namespace OT { %extend TruncatedNormal { TruncatedNormal(const TruncatedNormal & other) { return new OT::TruncatedNormal(other); } } }
