// SWIG file LinearModelFactory.i

%{
#include "LinearModelFactory.hxx"
%}

%include LinearModelFactory_doc.i

%include LinearModelFactory.hxx
namespace OT{ %extend LinearModelFactory { LinearModelFactory(const LinearModelFactory & other) { return new OT::LinearModelFactory(other); } } }
