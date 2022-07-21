// SWIG file KarhunenLoeveValidation.i

%{
#include "openturns/KarhunenLoeveValidation.hxx"
%}

%include KarhunenLoeveValidation_doc.i

%include openturns/KarhunenLoeveValidation.hxx
namespace OT { %extend KarhunenLoeveValidation { KarhunenLoeveValidation(const KarhunenLoeveValidation & other) { return new OT::KarhunenLoeveValidation(other); } } }
