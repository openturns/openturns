// SWIG file PersistentObject.i

%{
#include "openturns/PersistentObject.hxx"
%}

%ignore OT::PersistentObject::operator=(const PersistentObject &);
%ignore OT::PersistentObject::is;
%ignore OT::PersistentObject::getShadowedId;
%ignore OT::PersistentObject::setShadowedId;
%ignore OT::PersistentObject::hasVisibleName;
%ignore OT::PersistentObject::getVisibility;
%ignore OT::PersistentObject::setVisibility;
%ignore OT::PersistentObject::getId;

%include PersistentObject_doc.i

%include openturns/PersistentObject.hxx
