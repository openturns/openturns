// SWIG file PersistentObject.i

%{
#include "PersistentObject.hxx"
%}

%ignore OT::PersistentObject::operator=(const PersistentObject &);
%ignore OT::PersistentObject::is;

%include PersistentObject_doc.i

%include PersistentObject.hxx
