// SWIG file LinearModelFactory.i

%{
#include "openturns/LinearModelFactory.hxx"
%}

%include LinearModelFactory_doc.i

%include openturns/LinearModelFactory.hxx
namespace OT{ %extend LinearModelFactory { LinearModelFactory(const LinearModelFactory & other) { return new OT::LinearModelFactory(other); } } }
