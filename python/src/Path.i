// SWIG file Path.i

%{
#include "Path.hxx"
%}

%template(DirectoryList) std::vector<OT::FileName>;

%nodefaultctor Path;

%include Path.hxx
