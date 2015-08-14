// SWIG file InterfaceObject.i

%{
#include "InterfaceObject.hxx"
%}
%include InterfaceObject_doc.i

%ignore OT::InterfaceObject::setImplementationAsPersistentObject;
%ignore OT::InterfaceObject::getImplementationAsPersistentObject;

%include InterfaceObject.hxx
