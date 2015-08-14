// SWIG file IntervalMesher.i

%{
#include "IntervalMesher.hxx"
%}

%include IntervalMesher.hxx


%include IntervalMesher_doc.i

namespace OT {%extend IntervalMesher {IntervalMesher(const IntervalMesher & other){return new OT::IntervalMesher(other);}}}
