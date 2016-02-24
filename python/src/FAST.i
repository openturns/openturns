// SWIG file FAST.i

%{
#include "openturns/FAST.hxx"
%}

%include FAST_doc.i

%include openturns/FAST.hxx
namespace OT{ %extend FAST { FAST(const FAST & other) { return new OT::FAST(other); } } }
