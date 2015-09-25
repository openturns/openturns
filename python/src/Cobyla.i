// SWIG file Cobyla.i

%{
#include "Cobyla.hxx"
%}

%include Cobyla_doc.i

%include Cobyla.hxx
namespace OT{ %extend Cobyla { Cobyla(const Cobyla & other) { return new OT::Cobyla(other); } } }
