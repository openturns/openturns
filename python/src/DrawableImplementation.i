// SWIG file DrawableImplementation.i

%{
#include "openturns/DrawableImplementation.hxx"
%}

%include DrawableImplementation_doc.i

%include openturns/DrawableImplementation.hxx
namespace OT { %extend DrawableImplementation { DrawableImplementation(const DrawableImplementation & other) { return new OT::DrawableImplementation(other); } } }
