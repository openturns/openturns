// SWIG file Fehlberg.i

%{
#include "openturns/Fehlberg.hxx"
%}

%include Fehlberg_doc.i

%include openturns/Fehlberg.hxx
namespace OT { %extend Fehlberg { Fehlberg(const Fehlberg & other) { return new OT::Fehlberg(other); } } }
