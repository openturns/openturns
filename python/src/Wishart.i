// SWIG file Wishart.i

%{
#include "openturns/Wishart.hxx"
%}

%include Wishart_doc.i

%include openturns/Wishart.hxx
namespace OT { %extend Wishart { Wishart(const Wishart & other) { return new OT::Wishart(other); } } }
