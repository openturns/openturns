// SWIG file GraphImplementation.i

%{
#include "openturns/GraphImplementation.hxx"
%}

%include GraphImplementation_doc.i

%ignore OT::GraphImplementation::addDrawable(const OT::Drawable&);
%ignore OT::GraphImplementation::addDrawables(const DrawableCollection&);

%include openturns/GraphImplementation.hxx

namespace OT{ %extend GraphImplementation { GraphImplementation(const GraphImplementation & other) { return new OT::GraphImplementation(other); } } }
