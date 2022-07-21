/*                                               -*- C -*-  */
/**
 * @brief Python module to wrap Point and Sample without copy
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>

#define OT_BUFFER_MAX_DIMS 10

typedef struct
{
  double *data;
  Py_ssize_t itemsize;
  Py_ssize_t length;
  Py_ssize_t shape[OT_BUFFER_MAX_DIMS];
  Py_ssize_t strides[OT_BUFFER_MAX_DIMS];
  int ndim;
  char own;
} BufferView;

typedef struct
{
  PyObject_HEAD
  BufferView bufferview;
} Buffer;


static int
Buffer_init(Buffer *self, PyObject *args, PyObject *kwds)
{
  PyObject * addrObj = NULL;
  PyObject * shapeObj = NULL;
  PyObject * ownObj = NULL;
  Py_ssize_t shapeLength;
  Py_ssize_t bufferLength;
  Py_ssize_t i;
  char own;
  double * data = NULL;

  /* Constructor is never called directly by user, only from PythonEvaluation or when unpickling */
  (void) kwds;

  memset(&self->bufferview, 0, sizeof(BufferView));
  if (PyArg_ParseTuple(args, (char*) "n:Buffer", &bufferLength))
  {
    /* Unpickling: allocate an empty data memory block, it will be filled up by Buffer_setstate */
    self->bufferview.itemsize = sizeof(double);
    self->bufferview.length = bufferLength / self->bufferview.itemsize;
    self->bufferview.data = (double*) calloc(self->bufferview.length, self->bufferview.itemsize);
    self->bufferview.own = '\1';
    return 0;
  }
  PyErr_Clear();

  if (! PyArg_ParseTuple(args, (char*) "(OO!)O!:Buffer", &addrObj, &PyBool_Type, &ownObj, &PyTuple_Type, &shapeObj))
  {
    return -1;
  }

  data = (double*) PyLong_AsVoidPtr(addrObj);

  own = PyObject_IsTrue(ownObj);

  shapeLength = PyTuple_Size(shapeObj);
  if (shapeLength < 0 || shapeLength >= OT_BUFFER_MAX_DIMS)
  {
    return -1;
  }

  self->bufferview.own = own;
  self->bufferview.itemsize = sizeof(double);
  self->bufferview.ndim = (int)shapeLength;
  if (shapeLength == 0)
  {
    /* Is this really useful?  Maybe we should return -1 */
    self->bufferview.data = data;
    self->bufferview.shape[0] = 0;
    self->bufferview.strides[0] = self->bufferview.itemsize;
    self->bufferview.length = 0;
    return 0;
  }

  for(i = 0; i < shapeLength; ++i)
  {
    self->bufferview.shape[i] = PyLong_AsLong(PyTuple_GET_ITEM(shapeObj, i));
  }

  self->bufferview.strides[shapeLength - 1] = self->bufferview.itemsize;
  for(i = shapeLength - 2; i >= 0; --i)
    self->bufferview.strides[i] = self->bufferview.strides[i + 1] * self->bufferview.shape[i + 1];

  self->bufferview.length = self->bufferview.shape[0] * self->bufferview.strides[0] / self->bufferview.itemsize;
  if (self->bufferview.own == '\0')
  {
    self->bufferview.data = data;
  }
  else
  {
    self->bufferview.data = (double*) calloc(self->bufferview.length, self->bufferview.itemsize);
    memcpy(self->bufferview.data, data, self->bufferview.length * self->bufferview.itemsize);
  }

  return 0;
}

static void
Buffer_dealloc(Buffer* self)
{
  if (self->bufferview.own)
    free(self->bufferview.data);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static char*
dim_repr(Py_ssize_t* dims, int size)
{
  /* log10(2**63) = 18, and one for comma */
  char* s = malloc(size*19);
  char *ptr = s;
  int i;

  ptr += sprintf(s, "%ld", (long)dims[0]);
  for(i = 1; i < size; ++i)
    ptr += sprintf(ptr, ",%ld", (long)dims[i]);
  return s;
}

static PyObject *
Buffer_repr(Buffer * self)
{
  char * r = dim_repr(self->bufferview.shape, self->bufferview.ndim);
  char * s = malloc(100 + strlen(r));
  PyObject * result = NULL;

  sprintf(s, "<read-only buffer at %p shape=(%s)>", self->bufferview.data, r);
  free(r);

  result = PyUnicode_DecodeUTF8(s, (Py_ssize_t) strlen(s), "surrogateescape");
  free(s);
  return result;
}

static PyObject *
Buffer_iter(PyObject *obj)
{
  Buffer* self = (Buffer*)obj;

  if (self->bufferview.ndim == 0)
  {
    PyErr_SetString(PyExc_TypeError, "cannot iterate over a 0-d array");
    return NULL;
  }
  return PySeqIter_New(obj);
}

static int
Buffer_getbuffer(PyObject *obj, Py_buffer *view, int flags)
{
  (void)flags;
  Buffer* self = (Buffer*)obj;
  if (view == NULL) {
    PyErr_SetString(PyExc_ValueError, "NULL view in getbuffer");
    return -1;
  }

  view->obj = (PyObject*)self;
  view->buf = (void*)self->bufferview.data;
  view->len = self->bufferview.length * self->bufferview.itemsize;
  view->readonly = 1;
  view->itemsize = self->bufferview.itemsize;
  view->format = (char *)"d";
  view->ndim = self->bufferview.ndim;
  view->shape = self->bufferview.shape;
  view->strides = self->bufferview.strides;
  view->suboffsets = NULL;
  view->internal = NULL;

  Py_INCREF(self);
  return 0;
}

static PyBufferProcs Buffer_as_buffer = {
  (getbufferproc)     Buffer_getbuffer,
  (releasebufferproc) 0
};

/* Forward declaration.
   This method is defined after Buffer_item because it does the opposite job. */
static PyObject * Buffer_augment(PyObject *obj, PyObject *args);


/* This routine is used for pickling; it must return a tuple with 2 to 5 arguments:
   + obj: a callable object, which knows how to reconstruct the object.  We use
     the Buffer class itself, Buffer_init can create empty objects.
   + args: arguments passed to obj (we pass array length to check consistency in setstate)
   + state (optional): arguments passed to Buffer_setstate
   + 2 other arguments, not used here
 */
static PyObject *
Buffer_reduce(PyObject *obj, PyObject *args)
{
  Buffer* self = (Buffer*)obj;
  int i;
  PyObject * shapeTuple = NULL;
  PyObject * strideTuple = NULL;
  PyObject * rawObj = NULL;
  (void) args;

  shapeTuple = PyTuple_New(self->bufferview.ndim);
  if (shapeTuple == NULL)
  {
    return NULL;
  }
  for(i = 0; i < self->bufferview.ndim; ++i)
    PyTuple_SET_ITEM(shapeTuple, i, PyLong_FromSsize_t(self->bufferview.shape[i]));

  strideTuple = PyTuple_New(self->bufferview.ndim);
  if (strideTuple == NULL)
  {
    Py_DECREF(shapeTuple);
    return NULL;
  }
  for(i = 0; i < self->bufferview.ndim; ++i)
    PyTuple_SET_ITEM(strideTuple, i, PyLong_FromSsize_t(self->bufferview.strides[i]));

  rawObj = PyBytes_FromStringAndSize((const char*)self->bufferview.data, self->bufferview.itemsize * self->bufferview.length);

  return Py_BuildValue("O(n)(NNN)",
             /* We use Buffer type to build its own instance */
             (PyObject*) Py_TYPE(self),
             /* Arguments passed to object above, converted into a tuple */
             self->bufferview.length * self->bufferview.itemsize,
             /* Tuple arguments passed to Buffer_setstate */
             shapeTuple, strideTuple, rawObj);
}

static PyObject *
Buffer_setstate(PyObject *obj, PyObject *args)
{
  Buffer* self = (Buffer*)obj;
  PyObject * shapeObj = NULL;
  PyObject * strideObj = NULL;
  char * rawData;
  Py_ssize_t length = 0;
  Py_ssize_t shapeLength;
  Py_ssize_t strideLength;
  int i;

  if (!self->bufferview.own)
  {
    PyErr_SetString(PyExc_TypeError, "cannot populate a Buffer we do not own");
    return NULL;
  }
  if (! PyArg_ParseTuple(args, (char*) "(O!O!s#):__setstate__", &PyTuple_Type, &shapeObj, &PyTuple_Type, &strideObj, &rawData, &length))
    return NULL;

  if (length != self->bufferview.length * self->bufferview.itemsize)
  {
    PyErr_SetString(PyExc_TypeError, "invalid dimensions");
    return NULL;
  }
  shapeLength = PyTuple_Size(shapeObj);
  strideLength = PyTuple_Size(strideObj);
  if (shapeLength < 0 || shapeLength >= OT_BUFFER_MAX_DIMS || strideLength < 0 || strideLength >= OT_BUFFER_MAX_DIMS || shapeLength != strideLength)
  {
    return NULL;
  }
  self->bufferview.ndim = (int)shapeLength;
  for(i = 0; i < shapeLength; ++i)
  {
    self->bufferview.shape[i] = PyLong_AsLong(PyTuple_GET_ITEM(shapeObj, i));
    self->bufferview.strides[i] = PyLong_AsLong(PyTuple_GET_ITEM(strideObj, i));
  }
  memcpy(self->bufferview.data, rawData, length);
  Py_RETURN_NONE;
}

static struct PyMethodDef Buffer_methods[] = {
    {"augment", (PyCFunction)Buffer_augment, METH_NOARGS, NULL},
    /* These ones are required to pickle/unpickle */
    {"__reduce__", (PyCFunction)Buffer_reduce, METH_VARARGS, NULL},
    {"__setstate__", (PyCFunction)Buffer_setstate, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

/* Forward declaration */
static PySequenceMethods Buffer_as_sequence;

static const char Buffer_doc[] =                                          \
"openturns.memoryview.Buffer class.\n\n"                                  \
"This class allows wrapping OpenTURNS containers (Point, Sample, etc.)\n" \
"into Python objects without copy.  A Buffer object can be indexed,\n"    \
"or converted into a Sample or a numpy array.";

static PyTypeObject BufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "openturns.memoryview.Buffer",
    .tp_basicsize = sizeof(Buffer),
    .tp_dealloc = (destructor)Buffer_dealloc,
    .tp_repr = (reprfunc) Buffer_repr,
    .tp_as_sequence = &Buffer_as_sequence,
    .tp_str = (reprfunc) Buffer_repr,
    .tp_as_buffer = &Buffer_as_buffer,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = Buffer_doc,
    .tp_iter = (getiterfunc) Buffer_iter,
    .tp_methods = Buffer_methods,
    .tp_init = (initproc)Buffer_init,
};

static Py_ssize_t
Buffer_length(PyObject *obj)
{
  Buffer* self = (Buffer*)obj;
  if (self->bufferview.ndim == 0)
    return 0;
  return self->bufferview.shape[0];
}

static PyObject *
Buffer_item(PyObject *obj, Py_ssize_t index)
{
  int i;
  PyObject * newView;
  Buffer * newSelf;

  Buffer* self = (Buffer*)obj;
  if (self->bufferview.ndim == 0 || index < 0 || index >= self->bufferview.shape[0])
  {
    PyErr_SetString(PyExc_IndexError, "Buffer index out of range");
    return NULL;
  }
  else if (self->bufferview.ndim == 1)
  {
    double value = self->bufferview.data[index];
    return PyFloat_FromDouble(value);
  }

  /* Create another view of the same object with these changes:
       + data points to &data[index]
       + length is updated
       + ndim is decremented
       + shape is shifted: shape[i] = shape[i+1]
       + strides is shifted: strides[i] = strides[i+1]   */
  newView = PyType_GenericAlloc(&BufferType, 1);
  newSelf = (Buffer*)newView;
  memcpy(&newSelf->bufferview, &self->bufferview, sizeof(BufferView));
  newSelf->bufferview.data += index * newSelf->bufferview.strides[0] / newSelf->bufferview.itemsize;
  newSelf->bufferview.length /= self->bufferview.shape[0];
  --newSelf->bufferview.ndim;
  for(i = 0; i < newSelf->bufferview.ndim; ++i)
  {
    newSelf->bufferview.shape[i] = self->bufferview.shape[i + 1];
    newSelf->bufferview.strides[i] = self->bufferview.strides[i + 1];
  }
  return newView;
}

static PyObject *
Buffer_augment(PyObject *obj, PyObject *args)
{
  (void)args;
  int j;
  Buffer* self = (Buffer*)obj;
  PyObject * newView;
  Buffer * newSelf;

  if (self->bufferview.ndim == 0)
  {
    PyErr_SetString(PyExc_IndexError, "Cannot augment an empty Buffer");
    return NULL;
  }
  if (self->bufferview.ndim >= OT_BUFFER_MAX_DIMS)
  {
    PyErr_SetString(PyExc_IndexError, "Buffer maximum dimension reached");
    return NULL;
  }

  /* Create another view of the same object with some changes:
       + data points to data[index]
       + length is recomputed
       + ndim is incremented
       + shape is shifted: shape[i+1] = shape[i]
       + strides is shifted: strides[i+1] = strides[i]   */
  newView = PyType_GenericAlloc(&BufferType, 1);
  newSelf = (Buffer*)newView;
  memcpy(&newSelf->bufferview, &self->bufferview, sizeof(BufferView));

  for(j = newSelf->bufferview.ndim; j > 0; --j)
  {
    newSelf->bufferview.shape[j] = self->bufferview.shape[j - 1];
    newSelf->bufferview.strides[j] = self->bufferview.strides[j - 1];
  }
  newSelf->bufferview.shape[0] = 1;
  newSelf->bufferview.strides[0] = newSelf->bufferview.strides[1];
  ++newSelf->bufferview.ndim;
  return newView;
}

static PySequenceMethods Buffer_as_sequence = {
    .sq_length = (lenfunc)Buffer_length,
    .sq_concat = (binaryfunc)NULL,
    .sq_repeat = (ssizeargfunc)NULL,
    .sq_item = (ssizeargfunc)Buffer_item,
    .sq_ass_item = (ssizeobjargproc)NULL,
    .sq_contains = (objobjproc)NULL,
    .sq_inplace_concat = (binaryfunc) NULL,
    .sq_inplace_repeat = (ssizeargfunc)NULL,
};


#define openturns_memoryview_module_name "openturns.memoryview"

/*
  Module creation, adapted from https://docs.python.org/3/howto/cporting.html

  Minor changes may be required in PyModuleDef definition to support Python 3.0 and 3.1.
*/
struct module_state {
    PyObject *error;
};


#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

static PyObject *
error_out(PyObject *m, PyObject *args) {
    (void)args;
    struct module_state *st = GETSTATE(m);
    PyErr_SetString(st->error, "something bad happened");
    return NULL;
}

static PyMethodDef memoryview_methods[] = {
    {"error_out", (PyCFunction)error_out, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static int memoryview_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int memoryview_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef openturns_memoryview_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = openturns_memoryview_module_name,
    .m_doc = Buffer_doc,
    .m_size = sizeof(struct module_state),
    .m_methods = memoryview_methods,
    .m_slots = NULL,
    .m_traverse = memoryview_traverse,
    .m_clear = memoryview_clear,
    .m_free = NULL,
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_memoryview(void);

PyMODINIT_FUNC
PyInit_memoryview(void)
{
  PyObject *module;
  struct module_state *st;

  BufferType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&BufferType) < 0)
      INITERROR;

  module = PyModule_Create(&openturns_memoryview_module);

  if (module == NULL)
      INITERROR;

  st = GETSTATE(module);
  st->error = PyErr_NewException(openturns_memoryview_module_name ".Error", NULL, NULL);
  if (st->error == NULL) {
      Py_DECREF(module);
      INITERROR;
  }

  Py_INCREF(&BufferType);
  PyModule_AddObject(module, "Buffer", (PyObject *)&BufferType);

  return module;
}
