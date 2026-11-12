// SWIG file SparseMatrix.i

%{
#include "openturns/SparseMatrix.hxx"
%}

%include SparseMatrix_doc.i

%copyctor OT::SparseMatrix;

// use __getitem__ / __setitem__ instead
%ignore OT::SparseMatrix::operator();

%include openturns/SparseMatrix.hxx

namespace OT {

%extend SparseMatrix {

  // used by the Python __getitem__ / __setitem__ below
  Scalar _getValue(const UnsignedInteger i, const UnsignedInteger j) const { return static_cast<const OT::SparseMatrix &>(*self)(i, j); }

  void _setValue(const UnsignedInteger i, const UnsignedInteger j, const Scalar value) { (*self)(i, j) = value; }

} // SparseMatrix
} // OT

%pythoncode %{
def _SparseMatrix___getitem__(self, key):
    """Access a coefficient by (row, column) index."""
    i, j = key
    return self._getValue(i, j)


def _SparseMatrix___setitem__(self, key, value):
    """Set a coefficient by (row, column) index."""
    i, j = key
    self._setValue(i, j, value)


SparseMatrix.__getitem__ = _SparseMatrix___getitem__
SparseMatrix.__setitem__ = _SparseMatrix___setitem__
%}
