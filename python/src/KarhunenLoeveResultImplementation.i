// SWIG file KarhunenLoeveResultImplementation.i

%{
#include "openturns/KarhunenLoeveResultImplementation.hxx"
%}

%include KarhunenLoeveResultImplementation_doc.i

%include openturns/KarhunenLoeveResultImplementation.hxx
namespace OT { %extend KarhunenLoeveResultImplementation { KarhunenLoeveResultImplementation(const KarhunenLoeveResultImplementation & other) { return new OT::KarhunenLoeveResultImplementation(other); } } }
