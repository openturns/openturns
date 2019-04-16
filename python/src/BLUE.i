// SWIG file BLUE.i

%{
#include "openturns/BLUE.hxx"
%}

%include BLUE_doc.i

%include openturns/BLUE.hxx
namespace OT { %extend BLUE { BLUE(const BLUE & other) { return new OT::BLUE(other); } } }
