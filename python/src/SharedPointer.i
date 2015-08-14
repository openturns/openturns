// SWIG file SharedPointer.i

%{
#include "SharedPointer.hxx"
%}

%include SharedPointer.hxx
namespace OT{ %extend SharedPointer { SharedPointer(const SharedPointer & other) { return new OT::SharedPointer(other); } } }
