// SWIG file LevelSet.i

%{
#include "openturns/LevelSet.hxx"
%}

%include LevelSet_doc.i

%include openturns/LevelSet.hxx

namespace OT { %extend LevelSet { LevelSet(const LevelSet & other) { return new OT::LevelSet(other); } } }
