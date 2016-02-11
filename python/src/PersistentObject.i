// SWIG file PersistentObject.i

%{
#include "openturns/PersistentObject.hxx"
%}

%ignore OT::PersistentObject::operator=(const PersistentObject &);
%ignore OT::PersistentObject::is;

%include PersistentObject_doc.i

%include openturns/PersistentObject.hxx
