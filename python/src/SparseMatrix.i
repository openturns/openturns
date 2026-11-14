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

  PyObject * __getitem__(PyObject * key) const {
    Py_ssize_t rowIndex = 0;
    Py_ssize_t columnIndex = 0;
    if (!PyTuple_Check(key))
      throw OT::InvalidArgumentException(HERE) << "The coefficient index must be a (row, column) tuple";
    if (!PyArg_ParseTuple(key, "nn:SparseMatrix.__getitem__", &rowIndex, &columnIndex))
      return NULL;
    // negative indices are counted from the end, as for a dense matrix
    if (rowIndex < 0)
      rowIndex += static_cast<Py_ssize_t>(self->getNbRows());
    if (columnIndex < 0)
      columnIndex += static_cast<Py_ssize_t>(self->getNbColumns());
    if ((rowIndex < 0) || (columnIndex < 0))
      throw OT::OutOfBoundException(HERE) << "Invalid negative coefficient index (" << rowIndex << ", " << columnIndex << ")";
    return OT::convert<OT::Scalar, OT::_PyFloat_>((*self)(static_cast<OT::UnsignedInteger>(rowIndex), static_cast<OT::UnsignedInteger>(columnIndex)));
  }

  PyObject * __setitem__(PyObject * key, PyObject * valueObject) {
    Py_ssize_t rowIndex = 0;
    Py_ssize_t columnIndex = 0;
    if (!PyTuple_Check(key))
      throw OT::InvalidArgumentException(HERE) << "The coefficient index must be a (row, column) tuple";
    if (!PyArg_ParseTuple(key, "nn:SparseMatrix.__setitem__", &rowIndex, &columnIndex))
      return NULL;
    // negative indices are counted from the end, as for a dense matrix
    if (rowIndex < 0)
      rowIndex += static_cast<Py_ssize_t>(self->getNbRows());
    if (columnIndex < 0)
      columnIndex += static_cast<Py_ssize_t>(self->getNbColumns());
    if ((rowIndex < 0) || (columnIndex < 0))
      throw OT::OutOfBoundException(HERE) << "Invalid negative coefficient index (" << rowIndex << ", " << columnIndex << ")";
    (*self)(static_cast<OT::UnsignedInteger>(rowIndex), static_cast<OT::UnsignedInteger>(columnIndex)) = OT::convert<OT::_PyFloat_, OT::Scalar>(valueObject);
    return SWIG_Py_Void();
  }

} // SparseMatrix
} // OT
