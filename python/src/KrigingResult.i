// SWIG file KrigingResult.i

%{
#include "openturns/KrigingResult.hxx"
%}

%include KrigingResult_doc.i

%include openturns/KrigingResult.hxx

namespace OT{ %extend KrigingResult { KrigingResult(const KrigingResult & other) { return new OT::KrigingResult(other); } } }
