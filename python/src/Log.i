// SWIG file Log.i

%{
#include "Log.hxx"
%}

%apply UnsignedInteger { OT::Log::Severity };

%nodefaultctor Log;

%include Log.hxx
