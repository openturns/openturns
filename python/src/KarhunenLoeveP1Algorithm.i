// SWIG file KarhunenLoeveP1Algorithm.i

%{
#include "openturns/KarhunenLoeveP1Algorithm.hxx"
%}

%include KarhunenLoeveP1Algorithm_doc.i

%include openturns/KarhunenLoeveP1Algorithm.hxx
namespace OT { %extend KarhunenLoeveP1Algorithm { KarhunenLoeveP1Algorithm(const KarhunenLoeveP1Algorithm & other) { return new OT::KarhunenLoeveP1Algorithm(other); } } }
