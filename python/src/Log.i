// SWIG file Log.i

%{
#include "openturns/Log.hxx"
%}

%include Log_doc.i

%apply UnsignedInteger { OT::Log::Severity };

%nodefaultctor Log;

%include openturns/Log.hxx
