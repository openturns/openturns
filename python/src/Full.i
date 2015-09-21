// SWIG file Full.i

%{
#include "Full.hxx"
%}

%include Full_doc.i

%include Full.hxx
namespace OT{ %extend Full { Full(const Full & other) { return new OT::Full(other); } } }
