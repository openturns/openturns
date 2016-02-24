// SWIG file LAR.i

%{
#include "openturns/LAR.hxx"
%}

%include LAR_doc.i

%include openturns/LAR.hxx
namespace OT { %extend LAR { LAR(const LAR & other) { return new OT::LAR(other); } } }
