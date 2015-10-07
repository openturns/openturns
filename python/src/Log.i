// SWIG file Log.i

%{
#include "Log.hxx"
%}

%include Log_doc.i

%apply UnsignedInteger { OT::Log::Severity };

%nodefaultctor Log;

%include Log.hxx
