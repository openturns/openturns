// SWIG file DataContainer.i

%ignore OT::DataContainer::StorageBackend;
%ignore OT::DataContainer::MemoryBackend;
%ignore OT::DataContainer::getBackend;
%ignore OT::DataContainer::setBackend;
%ignore OT::DataContainer::clone;
%ignore OT::DataContainer::save;
%ignore OT::DataContainer::load;
%ignore OT::DataContainer::data;
%ignore OT::DataContainer::operator();
%ignore OT::DataContainer::operator[];
%ignore OT::DataContainer::at;
%ignore OT::DataContainer::backendSizeDebug;
%ignore OT::operator << (OStream &, const DataContainer &);
%ignore OT::operator << (std::ostream &, const DataContainer &);

%{
#include "openturns/DataContainer.hxx"
#include <sstream>
%}

%include DataContainer_doc.i

%include openturns/DataContainer.hxx

namespace OT {

%extend DataContainer {

String __repr__() const
{
  std::ostringstream oss;
  oss << "DataContainer(" << self->getSize() << ", " << self->getDimension() << ")";
  return oss.str();
}

Scalar __getitem__(UnsignedInteger i) const
{
  return self->at(i);
}

void __setitem__(UnsignedInteger i, Scalar val)
{
  self->at(i) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize() * self->getDimension();
}

} // %extend

} // OT
