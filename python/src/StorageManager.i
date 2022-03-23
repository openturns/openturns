// SWIG file StorageManager.i

%{
#include "openturns/StorageManager.hxx"
%}

%include StorageManager_doc.i

%ignore OT::LoadAction;
%ignore OT::SaveAction;

%include openturns/StorageManager.hxx
