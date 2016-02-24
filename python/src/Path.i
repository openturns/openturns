// SWIG file Path.i

%{
#include "openturns/Path.hxx"
%}

%include Path_doc.i

%template(DirectoryList) std::vector<OT::FileName>;

%nodefaultctor Path;

%include openturns/Path.hxx
