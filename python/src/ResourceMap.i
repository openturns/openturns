// SWIG file ResourceMap.i

%{
#include "openturns/ResourceMap.hxx"
%}

%include ResourceMap_doc.i

%nodefaultctor ResourceMap;

%ignore OT::ResourceMap_init;

%include openturns/ResourceMap.hxx

namespace OT {

%extend ResourceMap {

static UnsignedInteger __len__()
{
  return OT::ResourceMap::GetSize();
}

} // ResourceMap

} // OT
