// SWIG file IntervalMesher.i

%{
#include "openturns/IntervalMesher.hxx"
%}

%include IntervalMesher_doc.i

%include openturns/IntervalMesher.hxx

namespace OT {%extend IntervalMesher {IntervalMesher(const IntervalMesher & other){return new OT::IntervalMesher(other);}}}
