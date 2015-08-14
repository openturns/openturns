// SWIG file LevelSet.i

%{
#include "LevelSet.hxx"
%}

%include LevelSet_doc.i

%include LevelSet.hxx

namespace OT { %extend LevelSet { LevelSet(const LevelSet & other) { return new OT::LevelSet(other); } } }
