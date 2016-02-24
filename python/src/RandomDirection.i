// SWIG file RandomDirection.i

%{
#include "openturns/RandomDirection.hxx"
%}

%include RandomDirection_doc.i

%include openturns/RandomDirection.hxx
namespace OT{ %extend RandomDirection { RandomDirection(const RandomDirection & other) { return new OT::RandomDirection(other); } } }
