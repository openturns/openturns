// SWIG file LogNormal.i

%{
#include "openturns/LogNormal.hxx"
%}

%include LogNormal_doc.i

%include openturns/LogNormal.hxx
namespace OT { %extend LogNormal { LogNormal(const LogNormal & other) { return new OT::LogNormal(other); } } }
