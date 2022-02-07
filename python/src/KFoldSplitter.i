// SWIG file

%{
#include "openturns/KFoldSplitter.hxx"
%}

%include KFoldSplitter_doc.i

%include openturns/KFoldSplitter.hxx
namespace OT { %extend KFoldSplitter { KFoldSplitter(const KFoldSplitter & other) { return new OT::KFoldSplitter(other); } } }
