// SWIG file PersistentCollection.i

%{
#include "openturns/PersistentCollection.hxx"
%}

%include openturns/PersistentCollection.hxx

%template(_UnsignedIntegerPersistentCollection) OT::PersistentCollection<OT::UnsignedInteger>;
