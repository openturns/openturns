// SWIG file PersistentCollection.i

%{
#include "openturns/PersistentCollection.hxx"
%}

%include openturns/PersistentCollection.hxx

%template(UnsignedIntegerPersistentCollection) OT::PersistentCollection<OT::UnsignedInteger>;
