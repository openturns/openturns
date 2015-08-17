// SWIG file Cobyla.i

%{
#include "Cobyla.hxx"
%}

%include Cobyla.hxx
namespace OT{ %extend Cobyla { Cobyla(const Cobyla & other) { return new OT::Cobyla(other); } } }
