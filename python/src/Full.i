// SWIG file Full.i

%{
#include "openturns/Full.hxx"
%}

%include Full_doc.i

%include openturns/Full.hxx
namespace OT{ %extend Full { Full(const Full & other) { return new OT::Full(other); } } }
