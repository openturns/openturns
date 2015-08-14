// SWIG file Brent.i

%{
#include "Brent.hxx"
%}

%include Brent.hxx
namespace OT { %extend Brent { Brent(const Brent & other) { return new OT::Brent(other); } } }
