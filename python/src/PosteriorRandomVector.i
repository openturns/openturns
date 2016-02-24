// SWIG file PosteriorRandomVector.i

%{
#include "openturns/PosteriorRandomVector.hxx"
%}

%include PosteriorRandomVector_doc.i

%include openturns/PosteriorRandomVector.hxx
namespace OT { %extend PosteriorRandomVector { PosteriorRandomVector(const PosteriorRandomVector & other) { return new OT::PosteriorRandomVector(other); } } }
