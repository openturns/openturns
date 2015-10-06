// SWIG file Path.i

%{
#include "Path.hxx"
%}

%include Path_doc.i

%template(DirectoryList) std::vector<OT::FileName>;

%nodefaultctor Path;

%include Path.hxx
