// SWIG file Path.i

%{
#include "openturns/Path.hxx"
%}

%include Path_doc.i

%template() std::vector<OT::FileName>;

%nodefaultctor Path;

%ignore OT::Path::FindFileByNameInDirectoryList;

%include openturns/Path.hxx
