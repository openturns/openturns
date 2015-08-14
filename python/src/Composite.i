// SWIG file Composite.i

%{
#include "Composite.hxx"
%}

%include Composite_doc.i

%include Composite.hxx
namespace OT { %extend Composite { Composite(const Composite & other) { return new OT::Composite(other); } } }
