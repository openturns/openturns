// SWIG file Weibull.i

%{
#include "openturns/Weibull.hxx"
%}

%include Weibull_doc.i

%include openturns/Weibull.hxx
namespace OT { %extend Weibull { Weibull(const Weibull & other) { return new OT::Weibull(other); } } }
