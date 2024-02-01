// SWIG file GraphImplementation.i

%{
#include "openturns/GraphImplementation.hxx"
%}

%include GraphImplementation_doc.i

%ignore OT::GraphImplementation::addDrawable(const OT::Drawable&);
%ignore OT::GraphImplementation::addDrawables(const DrawableCollection&);

%copyctor OT::GraphImplementation;

%include openturns/GraphImplementation.hxx
