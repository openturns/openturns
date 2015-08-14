// SWIG file Catalog.i

%{
#include "Catalog.hxx"
%}

%include Catalog.hxx
namespace OT {  
%extend Catalog {
  const String __str__() const { return self->__repr__(); }
}
}
