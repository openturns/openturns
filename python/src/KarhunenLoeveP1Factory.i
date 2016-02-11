// SWIG file KarhunenLoeveP1Factory.i

%{
#include "openturns/KarhunenLoeveP1Factory.hxx"
%}

%include KarhunenLoeveP1Factory_doc.i

%include openturns/KarhunenLoeveP1Factory.hxx
namespace OT { %extend KarhunenLoeveP1Factory { KarhunenLoeveP1Factory(const KarhunenLoeveP1Factory & other) { return new OT::KarhunenLoeveP1Factory(other); } } }
