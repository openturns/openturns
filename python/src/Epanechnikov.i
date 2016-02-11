// SWIG file Epanechnikov.i

%{
#include "openturns/Epanechnikov.hxx"
%}

%include Epanechnikov_doc.i

%include openturns/Epanechnikov.hxx
namespace OT { %extend Epanechnikov { Epanechnikov(const Epanechnikov & other) { return new OT::Epanechnikov(other); } } }
