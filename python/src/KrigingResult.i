// SWIG file KrigingResult.i

%{
#include "KrigingResult.hxx"
%}

%include KrigingResult_doc.i

%include KrigingResult.hxx

namespace OT{ %extend KrigingResult { KrigingResult(const KrigingResult & other) { return new OT::KrigingResult(other); } } }
