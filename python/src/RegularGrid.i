// SWIG file RegularGrid.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/RegularGrid.hxx"
%}


%include RegularGrid_doc.i

%include openturns/RegularGrid.hxx

namespace OT { %extend RegularGrid { RegularGrid(const RegularGrid & other) { return new OT::RegularGrid(other); } } }
