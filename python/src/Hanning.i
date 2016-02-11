// SWIG file Hanning.i

%{
#include "openturns/Hanning.hxx"
%}


%include Hanning_doc.i

%include openturns/Hanning.hxx

namespace OT { %extend Hanning { Hanning(const Hanning & other) { return new OT::Hanning(other); } } }
