// SWIG file Wilks.i

%{
#include "Wilks.hxx"
%}

%include Wilks_doc.i

%include Wilks.hxx
namespace OT { %extend Wilks { Wilks(const Wilks & other) { return new OT::Wilks(other); } } }
