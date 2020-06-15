// SWIG file GridLayout.i

%{
#include "openturns/GridLayout.hxx"
%}

%include GridLayout_doc.i

%include openturns/GridLayout.hxx

namespace OT { %extend GridLayout { GridLayout(const GridLayout & other) { return new OT::GridLayout(other); } } }  

