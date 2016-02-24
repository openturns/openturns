// SWIG file GumbelAB.i

%{
#include "openturns/GumbelAB.hxx"
%}

%include GumbelAB_doc.i

%include openturns/GumbelAB.hxx
namespace OT{ %extend GumbelAB { GumbelAB(const GumbelAB & other) { return new OT::GumbelAB(other); } } }
