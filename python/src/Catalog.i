// SWIG file Catalog.i

%{
#include "openturns/Catalog.hxx"
%}

%include openturns/Catalog.hxx
namespace OT {  
%extend Catalog {
  const String __str__() const { return self->__repr__(); }
}
}
