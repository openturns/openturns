// SWIG file Frechet.i

%{
#include "openturns/Frechet.hxx"
%}

%include Frechet_doc.i

%include openturns/Frechet.hxx
namespace OT { %extend Frechet { Frechet(const Frechet & other) { return new OT::Frechet(other); } } }
