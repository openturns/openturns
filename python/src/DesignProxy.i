// SWIG file DesignProxy.i

%{
#include "openturns/DesignProxy.hxx"
%}

%include DesignProxy_doc.i

%include openturns/DesignProxy.hxx
namespace OT { %extend DesignProxy { DesignProxy(const DesignProxy & other) { return new OT::DesignProxy(other); } } }

