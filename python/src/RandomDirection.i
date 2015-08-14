// SWIG file RandomDirection.i

%{
#include "RandomDirection.hxx"
%}

%include RandomDirection.hxx
namespace OT{ %extend RandomDirection { RandomDirection(const RandomDirection & other) { return new OT::RandomDirection(other); } } }
