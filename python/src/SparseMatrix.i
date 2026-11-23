// SWIG file SparseMatrix.i

%{
#include "openturns/SparseMatrix.hxx"
#include "openturns/Exception.hxx"

// Normalise a Python index against a dimension in an unsigned 64-bit domain:
// UnsignedInteger is unsigned long, which may be wider than Py_ssize_t, so
// casting the dimension to Py_ssize_t could make the upper bound negative for
// a huge dimension. The negative case adds one before negating so that the
// most negative index does not overflow.
static OT::UnsignedInteger SparseMatrixNormalizeIndex(long long index, OT::UnsignedInteger dimension)
{
  const OT::Unsigned64BitsInteger dimension64 = dimension;
  if (index < 0)
  {
    const OT::Unsigned64BitsInteger magnitude = static_cast<OT::Unsigned64BitsInteger>(-(index + 1)) + 1;
    if (magnitude > dimension64)
      throw OT::OutOfBoundException(HERE) << "Invalid negative coefficient index " << std::to_string(index) << " for a dimension of " << dimension;
    return static_cast<OT::UnsignedInteger>(dimension64 - magnitude);
  }
  if (static_cast<OT::Unsigned64BitsInteger>(index) >= dimension64)
    throw OT::OutOfBoundException(HERE) << "Invalid coefficient index " << std::to_string(index) << " for a dimension of " << dimension;
  return static_cast<OT::UnsignedInteger>(index);
}
%}

%include SparseMatrix_doc.i

%copyctor OT::SparseMatrix;

// use __getitem__ / __setitem__ instead
%ignore OT::SparseMatrix::operator();

%include openturns/SparseMatrix.hxx

namespace OT {

%extend SparseMatrix {

  PyObject * __getitem__(PyObject * key) const {
    Py_ssize_t rowIndex = 0;
    Py_ssize_t columnIndex = 0;
    if (!PyTuple_Check(key))
      throw OT::InvalidArgumentException(HERE) << "The coefficient index must be a (row, column) tuple";
    if (!PyArg_ParseTuple(key, "nn:SparseMatrix.__getitem__", &rowIndex, &columnIndex))
      return NULL;
    const OT::UnsignedInteger row = SparseMatrixNormalizeIndex(rowIndex, self->getNbRows());
    const OT::UnsignedInteger column = SparseMatrixNormalizeIndex(columnIndex, self->getNbColumns());
    return OT::convert<OT::Scalar, OT::_PyFloat_>((*self)(row, column));
  }

  PyObject * __setitem__(PyObject * key, PyObject * valueObject) {
    Py_ssize_t rowIndex = 0;
    Py_ssize_t columnIndex = 0;
    if (!PyTuple_Check(key))
      throw OT::InvalidArgumentException(HERE) << "The coefficient index must be a (row, column) tuple";
    if (!PyArg_ParseTuple(key, "nn:SparseMatrix.__setitem__", &rowIndex, &columnIndex))
      return NULL;
    const OT::UnsignedInteger row = SparseMatrixNormalizeIndex(rowIndex, self->getNbRows());
    const OT::UnsignedInteger column = SparseMatrixNormalizeIndex(columnIndex, self->getNbColumns());
    const OT::Scalar value = OT::convert<OT::_PyFloat_, OT::Scalar>(valueObject);
    // On a conversion failure convert() returns the failure value of
    // PyFloat_AsDouble() and leaves a Python error set: storing that value
    // would modify the matrix and insert an entry for a rejected assignment.
    if (PyErr_Occurred())
      return NULL;
    (*self)(row, column) = value;
    return SWIG_Py_Void();
  }

} // SparseMatrix
} // OT
