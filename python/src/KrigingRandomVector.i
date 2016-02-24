// SWIG file KrigingRandomVector.i

%{
#include "openturns/KrigingRandomVector.hxx"
%}

%include KrigingRandomVector_doc.i

%include openturns/KrigingRandomVector.hxx
namespace OT { %extend KrigingRandomVector { KrigingRandomVector(const KrigingRandomVector & other) { return new OT::KrigingRandomVector(other); } } }
