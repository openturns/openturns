// SWIG file Cloud.i

%{
#include "Cloud.hxx"
%}

%include Cloud_doc.i

%include Cloud.hxx
namespace OT { %extend Cloud { Cloud(const Cloud & other) { return new OT::Cloud(other); } } }
