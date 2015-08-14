// SWIG file Skellam.i

%{
#include "Skellam.hxx"
%}

%include Skellam_doc.i

%include Skellam.hxx
namespace OT { %extend Skellam { Skellam(const Skellam & other) { return new OT::Skellam(other); } } }
