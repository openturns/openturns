// SWIG file ArcsineFactory.i

%{
#include "ArcsineFactory.hxx"
%}

%include ArcsineFactory_doc.i

%include ArcsineFactory.hxx
namespace OT { %extend ArcsineFactory { ArcsineFactory(const ArcsineFactory & other) { return new OT::ArcsineFactory(other); } } }
