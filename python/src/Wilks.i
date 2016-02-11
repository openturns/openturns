// SWIG file Wilks.i

%{
#include "openturns/Wilks.hxx"
%}

%include Wilks_doc.i

%include openturns/Wilks.hxx
namespace OT { %extend Wilks { Wilks(const Wilks & other) { return new OT::Wilks(other); } } }
