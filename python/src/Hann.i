// SWIG file Hann.i

%{
#include "openturns/Hann.hxx"
%}


%include Hann_doc.i

%include openturns/Hann.hxx

namespace OT { %extend Hann { Hann(const Hann & other) { return new OT::Hann(other); } } }

