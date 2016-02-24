// SWIG file InterfaceObject.i

%{
#include "openturns/InterfaceObject.hxx"
%}
%include InterfaceObject_doc.i

%ignore OT::InterfaceObject::setImplementationAsPersistentObject;
%ignore OT::InterfaceObject::getImplementationAsPersistentObject;

%include openturns/InterfaceObject.hxx
