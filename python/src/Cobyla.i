// SWIG file Cobyla.i

%{
#include "openturns/Cobyla.hxx"
%}

%include Cobyla_doc.i

%include openturns/Cobyla.hxx
namespace OT{ %extend Cobyla { Cobyla(const Cobyla & other) { return new OT::Cobyla(other); } } }
