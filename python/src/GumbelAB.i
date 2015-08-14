// SWIG file GumbelAB.i

%{
#include "GumbelAB.hxx"
%}

%include GumbelAB_doc.i

%include GumbelAB.hxx
namespace OT{ %extend GumbelAB { GumbelAB(const GumbelAB & other) { return new OT::GumbelAB(other); } } }
