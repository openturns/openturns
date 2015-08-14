// SWIG file Hanning.i

%{
#include "Hanning.hxx"
%}


%include Hanning_doc.i

%include Hanning.hxx

namespace OT { %extend Hanning { Hanning(const Hanning & other) { return new OT::Hanning(other); } } }
