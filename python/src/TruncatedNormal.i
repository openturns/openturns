// SWIG file TruncatedNormal.i

%{
#include "openturns/TruncatedNormal.hxx"
%}

%include TruncatedNormal_doc.i

%include openturns/TruncatedNormal.hxx
namespace OT { %extend TruncatedNormal { TruncatedNormal(const TruncatedNormal & other) { return new OT::TruncatedNormal(other); } } }
