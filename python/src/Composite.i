// SWIG file Composite.i

%{
#include "openturns/Composite.hxx"
%}

%include Composite_doc.i

%include openturns/Composite.hxx
namespace OT { %extend Composite { Composite(const Composite & other) { return new OT::Composite(other); } } }
