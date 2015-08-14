// SWIG file Wishart.i

%{
#include "Wishart.hxx"
%}

%include Wishart.hxx
namespace OT { %extend Wishart { Wishart(const Wishart & other) { return new OT::Wishart(other); } } }
