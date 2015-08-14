// SWIG file Frechet.i

%{
#include "Frechet.hxx"
%}

%include Frechet_doc.i

%include Frechet.hxx
namespace OT { %extend Frechet { Frechet(const Frechet & other) { return new OT::Frechet(other); } } }
