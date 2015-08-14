// SWIG file KrigingRandomVector.i

%{
#include "KrigingRandomVector.hxx"
%}

%include KrigingRandomVector_doc.i

%include KrigingRandomVector.hxx
namespace OT { %extend KrigingRandomVector { KrigingRandomVector(const KrigingRandomVector & other) { return new OT::KrigingRandomVector(other); } } }
