// SWIG file XMLStorageManager.i

%{
#include "openturns/XMLStorageManager.hxx"
%}

%include XMLStorageManager_doc.i

%ignore OT::XMLStorageManager::GetNumericalPrecision;

%include openturns/XMLStorageManager.hxx
