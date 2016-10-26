// SWIG file KarhunenLoeveResult.i

%{
#include "openturns/KarhunenLoeveResult.hxx"
%}

%include KarhunenLoeveResult_doc.i

OTTypedInterfaceObjectHelper(KarhunenLoeveResult)

%include openturns/KarhunenLoeveResult.hxx
namespace OT { %extend KarhunenLoeveResult { KarhunenLoeveResult(const KarhunenLoeveResult & other) { return new OT::KarhunenLoeveResult(other); } } }
