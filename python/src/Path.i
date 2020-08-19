// SWIG file Path.i

%{
#include "openturns/Path.hxx"
%}

%include Path_doc.i

%template(DirectoryList) std::vector<OT::FileName>;

%nodefaultctor Path;

// these are available in tempfile or os
%ignore OT::Path::GetTemporaryDirectory;
%ignore OT::Path::BuildTemporaryFileName;
%ignore OT::Path::CreateTemporaryDirectory;
%ignore OT::Path::FindFileByNameInDirectoryList;

%include openturns/Path.hxx
