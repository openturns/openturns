// SWIG file KarhunenLoeveQuadratureAlgorithm.i

%{
#include "openturns/KarhunenLoeveQuadratureAlgorithm.hxx"
%}

%include KarhunenLoeveQuadratureAlgorithm_doc.i

%include openturns/KarhunenLoeveQuadratureAlgorithm.hxx
namespace OT { %extend KarhunenLoeveQuadratureAlgorithm { KarhunenLoeveQuadratureAlgorithm(const KarhunenLoeveQuadratureAlgorithm & other) { return new OT::KarhunenLoeveQuadratureAlgorithm(other); } } }
