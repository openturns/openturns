// SWIG file FAST.i

%{
#include "FAST.hxx"
%}

%include FAST_doc.i

%include FAST.hxx
namespace OT{ %extend FAST { FAST(const FAST & other) { return new OT::FAST(other); } } }
