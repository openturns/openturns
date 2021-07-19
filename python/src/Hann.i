// SWIG file Hann.i

%{
#include "openturns/Hann.hxx"
%}


%include Hann_doc.i

%include openturns/Hann.hxx

namespace OT { %extend Hann { Hann(const Hann & other) { return new OT::Hann(other); } } }

%pythoncode %{
def Hanning():
    openturns.common.Log.Warn('class Hanning is deprecated in favor of Hann')
    return openturns.statistics.Hann()
%}

