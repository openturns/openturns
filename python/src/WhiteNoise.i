// SWIG file WhiteNoise.i

%{
#include "WhiteNoise.hxx"
%}

%include WhiteNoise_doc.i


%include WhiteNoise.hxx
namespace OT { %extend WhiteNoise { WhiteNoise(const WhiteNoise & other) { return new OT::WhiteNoise(other); } } }
