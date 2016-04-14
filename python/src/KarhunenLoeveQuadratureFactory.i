// SWIG file KarhunenLoeveQuadratureFactory.i

%{
#include "openturns/KarhunenLoeveQuadratureFactory.hxx"
%}

%include KarhunenLoeveQuadratureFactory_doc.i

%include openturns/KarhunenLoeveQuadratureFactory.hxx
namespace OT { %extend KarhunenLoeveQuadratureFactory { KarhunenLoeveQuadratureFactory(const KarhunenLoeveQuadratureFactory & other) { return new OT::KarhunenLoeveQuadratureFactory(other); } } }
