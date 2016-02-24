// SWIG file Brent.i

%{
#include "openturns/Brent.hxx"
%}

%include Brent_doc.i

%include openturns/Brent.hxx
namespace OT { %extend Brent { Brent(const Brent & other) { return new OT::Brent(other); } } }
