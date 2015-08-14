// SWIG file Arcsine.i

%{
#include "Arcsine.hxx"
%}

%include Arcsine_doc.i

%include Arcsine.hxx
namespace OT { %extend Arcsine { Arcsine(const Arcsine & other) { return new OT::Arcsine(other); } } }
