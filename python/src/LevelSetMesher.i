// SWIG file LevelSetMesher.i

%{
#include "openturns/LevelSetMesher.hxx"
%}

%include LevelSetMesher_doc.i

%include openturns/LevelSetMesher.hxx

namespace OT {%extend LevelSetMesher {LevelSetMesher(const LevelSetMesher & other){return new OT::LevelSetMesher(other);}}}
