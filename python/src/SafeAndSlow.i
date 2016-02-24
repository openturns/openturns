// SWIG file SafeAndSlow.i

%{
#include "openturns/SafeAndSlow.hxx"
%}

%include SafeAndSlow_doc.i

%include openturns/SafeAndSlow.hxx
namespace OT{ %extend SafeAndSlow { SafeAndSlow(const SafeAndSlow & other) { return new OT::SafeAndSlow(other); } } }
