// SWIG file PersistentCollection.i

%{
#include "PersistentCollection.hxx"
%}

%include PersistentCollection.hxx

%template(UnsignedIntegerPersistentCollection) OT::PersistentCollection<OT::UnsignedInteger>;
