// SWIG file Histogram.i

%{
#include "openturns/Histogram.hxx"
%}

%include Histogram_doc.i

%include openturns/Histogram.hxx

namespace OT { %extend Histogram { Histogram(const Histogram & other) { return new OT::Histogram(other); } } }
