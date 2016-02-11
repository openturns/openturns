// SWIG file Compact.i

%{
#include "openturns/Compact.hxx"
%}

%include Compact_doc.i

%include openturns/Compact.hxx
namespace OT{ %extend Compact { Compact(const Compact & other) { return new OT::Compact(other); } } }
