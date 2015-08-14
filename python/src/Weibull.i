// SWIG file Weibull.i

%{
#include "Weibull.hxx"
%}

%include Weibull_doc.i

%include Weibull.hxx
namespace OT { %extend Weibull { Weibull(const Weibull & other) { return new OT::Weibull(other); } } }
