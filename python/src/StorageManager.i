// SWIG file StorageManager.i

%{
#include "openturns/StorageManager.hxx"
%}

%include StorageManager_doc.i

%ignore OT::LoadAction;
%ignore OT::SaveAction;
%ignore OT::StorageManager::markObjectAsSaved;
%ignore OT::StorageManager::isSavedObject;
%ignore OT::StorageManager::initialize;
%ignore OT::StorageManager::finalize;
%ignore OT::StorageManager::save;
%ignore OT::StorageManager::load;
%ignore OT::StorageManager::read;
%ignore OT::StorageManager::write;
%ignore OT::StorageManager::getStudy;
%ignore OT::StorageManager::setStudy;

%include openturns/StorageManager.hxx
