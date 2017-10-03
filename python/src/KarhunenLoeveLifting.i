// SWIG file KarhunenLoeveLifting.i

%{
#include "openturns/KarhunenLoeveLifting.hxx"
%}

%include KarhunenLoeveLifting_doc.i

%include openturns/KarhunenLoeveLifting.hxx
namespace OT { %extend KarhunenLoeveLifting { KarhunenLoeveLifting(const KarhunenLoeveLifting & other) { return new OT::KarhunenLoeveLifting(other); } } }
