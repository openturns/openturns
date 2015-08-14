// SWIG file PosteriorRandomVector.i

%{
#include "PosteriorRandomVector.hxx"
%}

%include PosteriorRandomVector_doc.i

%include PosteriorRandomVector.hxx
namespace OT { %extend PosteriorRandomVector { PosteriorRandomVector(const PosteriorRandomVector & other) { return new OT::PosteriorRandomVector(other); } } }
