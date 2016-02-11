// SWIG file Cloud.i

%{
#include "openturns/Cloud.hxx"
%}

%include Cloud_doc.i

%include openturns/Cloud.hxx
namespace OT { %extend Cloud { Cloud(const Cloud & other) { return new OT::Cloud(other); } } }
