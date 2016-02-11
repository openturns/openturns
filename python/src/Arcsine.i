// SWIG file Arcsine.i

%{
#include "openturns/Arcsine.hxx"
%}

%include Arcsine_doc.i

%include openturns/Arcsine.hxx
namespace OT { %extend Arcsine { Arcsine(const Arcsine & other) { return new OT::Arcsine(other); } } }
