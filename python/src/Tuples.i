// SWIG file Tuples.i

%{
#include "openturns/Tuples.hxx"
%}

%include Tuples_doc.i

%include openturns/Tuples.hxx
namespace OT { %extend Tuples { Tuples(const Tuples & other) { return new OT::Tuples(other); } } }
