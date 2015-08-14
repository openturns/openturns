// SWIG file DrawableImplementation.i

%{
#include "DrawableImplementation.hxx"
%}

%include DrawableImplementation_doc.i

%include DrawableImplementation.hxx
namespace OT { %extend DrawableImplementation { DrawableImplementation(const DrawableImplementation & other) { return new OT::DrawableImplementation(other); } } }
