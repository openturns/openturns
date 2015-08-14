// SWIG file SafeAndSlow.i

%{
#include "SafeAndSlow.hxx"
%}

%include SafeAndSlow_doc.i

%include SafeAndSlow.hxx
namespace OT{ %extend SafeAndSlow { SafeAndSlow(const SafeAndSlow & other) { return new OT::SafeAndSlow(other); } } }
