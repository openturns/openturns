// SWIG file Epanechnikov.i

%{
#include "Epanechnikov.hxx"
%}

%include Epanechnikov_doc.i

%include Epanechnikov.hxx
namespace OT { %extend Epanechnikov { Epanechnikov(const Epanechnikov & other) { return new OT::Epanechnikov(other); } } }
