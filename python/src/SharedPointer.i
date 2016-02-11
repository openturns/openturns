// SWIG file SharedPointer.i

%{
#include "openturns/SharedPointer.hxx"
%}

%include openturns/SharedPointer.hxx
namespace OT{ %extend SharedPointer { SharedPointer(const SharedPointer & other) { return new OT::SharedPointer(other); } } }
