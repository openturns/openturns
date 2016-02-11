// SWIG file Skellam.i

%{
#include "openturns/Skellam.hxx"
%}

%include Skellam_doc.i

%include openturns/Skellam.hxx
namespace OT { %extend Skellam { Skellam(const Skellam & other) { return new OT::Skellam(other); } } }
