// SWIG file KarhunenLoeveReduction.i

%{
#include "openturns/KarhunenLoeveReduction.hxx"
%}

%include KarhunenLoeveReduction_doc.i

%include openturns/KarhunenLoeveReduction.hxx
namespace OT { %extend KarhunenLoeveReduction { KarhunenLoeveReduction(const KarhunenLoeveReduction & other) { return new OT::KarhunenLoeveReduction(other); } } }


