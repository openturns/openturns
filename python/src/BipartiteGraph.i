// SWIG file BipartiteGraph.i

%{
#include "openturns/BipartiteGraph.hxx"
%}

%include BipartiteGraph_doc.i

%include openturns/BipartiteGraph.hxx

namespace OT {
%extend BipartiteGraph {

Indices __getitem__(SignedInteger index) const {
  OT::UnsignedInteger size = self->getSize();
  if (size == 0) throw OT::OutOfBoundException(HERE) << "collection is empty.";
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0 || index >= static_cast<OT::SignedInteger>(size)) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return OT::Indices(self->cbegin_at(index), self->cend_at(index));
}

void __setitem__ (SignedInteger index,
                  const Indices & val) {
  OT::UnsignedInteger size = self->getSize();
  if (size == 0) throw OT::OutOfBoundException(HERE) << "collection is empty.";
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0 || index >= static_cast<OT::SignedInteger>(size)) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  OT::UnsignedInteger thisSize = self->cend_at(index) - self->cbegin_at(index);
  if (val.getSize() != thisSize)
    throw OT::InvalidArgumentException(HERE) << "element at position " << index << " is of size " << thisSize << " whereas value size is " << val.getSize();
  std::copy(val.begin(), val.end(), self->begin_at(index));
}

UnsignedInteger __len__() const
{
  return self->getSize();
}

BipartiteGraph(const BipartiteGraph & other)
{
  return new OT::BipartiteGraph(other);
}

}
}
