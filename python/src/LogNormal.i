// SWIG file LogNormal.i

%{
#include "LogNormal.hxx"
%}

%include LogNormal_doc.i

%include LogNormal.hxx
namespace OT { %extend LogNormal { LogNormal(const LogNormal & other) { return new OT::LogNormal(other); } } }
