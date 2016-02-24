// SWIG file WhiteNoise.i

%{
#include "openturns/WhiteNoise.hxx"
%}

%include WhiteNoise_doc.i


%include openturns/WhiteNoise.hxx
namespace OT { %extend WhiteNoise { WhiteNoise(const WhiteNoise & other) { return new OT::WhiteNoise(other); } } }
