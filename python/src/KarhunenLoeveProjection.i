// SWIG file KarhunenLoeveProjection.i

%{
#include "openturns/KarhunenLoeveProjection.hxx"
%}

%include KarhunenLoeveProjection_doc.i

%include openturns/KarhunenLoeveProjection.hxx
namespace OT { %extend KarhunenLoeveProjection { KarhunenLoeveProjection(const KarhunenLoeveProjection & other) { return new OT::KarhunenLoeveProjection(other); } } }
