// SWIG file ArcsineFactory.i

%{
#include "openturns/ArcsineFactory.hxx"
%}

%include ArcsineFactory_doc.i

%include openturns/ArcsineFactory.hxx
namespace OT { %extend ArcsineFactory { ArcsineFactory(const ArcsineFactory & other) { return new OT::ArcsineFactory(other); } } }
