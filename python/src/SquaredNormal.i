// SWIG file SquaredNormal.i

%{
#include "openturns/SquaredNormal.hxx"
%}

%include SquaredNormal_doc.i

%include openturns/SquaredNormal.hxx
namespace OT { %extend SquaredNormal { SquaredNormal(const SquaredNormal & other) { return new OT::SquaredNormal(other); } } }
