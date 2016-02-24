// SWIG file Null.i

%{
#include "openturns/Null.hxx"
%}

%include Null_doc.i

%include openturns/Null.hxx
namespace OT{ %extend Null { Null(const Null & other) { return new OT::Null(other); } } }
