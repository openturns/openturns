// SWIG file HistogramPair.i

%{
#include "HistogramPair.hxx"

  namespace OT {

    template <>
      struct traitsPythonType<OT::HistogramPair>
      {
        typedef _PyObject_ Type;
      };

    template <>
      inline
      OT::HistogramPair *
      buildObjectFromPySequence<OT::HistogramPair>(PyObject * pyObj)
      {
        check<_PySequence_>( pyObj );

        const UnsignedInteger size = PySequence_Fast_GET_SIZE( pyObj );
        if (size != 2) {
          throw OT::InvalidArgumentException(HERE) << "Sequence object has incorrect size " << size << ". Must be 2.";
        }
        PyObject * elt1 = PySequence_Fast_GET_ITEM( pyObj, 0 );
        check<_PyFloat_>( elt1 );
        PyObject * elt2 = PySequence_Fast_GET_ITEM( pyObj, 1 );
        check<_PyFloat_>( elt2 );
        return new OT::HistogramPair( convert<_PyFloat_,OT::NumericalScalar>( elt1 ),
                                                                        convert<_PyFloat_,OT::NumericalScalar>( elt2 ) );
      }

    template <>
      inline
      OT::HistogramPair
      convert<_PySequence_,OT::HistogramPair>(PyObject * pyObj)
      {
        OT::Pointer<OT::HistogramPair> ptr = buildObjectFromPySequence<OT::HistogramPair>( pyObj );
        return *ptr;
      }

    template <>
      inline
      OT::HistogramPair
      convert<_PyObject_,OT::HistogramPair>(PyObject * pyObj)
      {
        void * ptr = 0;
        if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::HistogramPair *"), 0 |  0 ))) {
          OT::HistogramPair * p_hp = reinterpret_cast< OT::HistogramPair * >( ptr );
          return *p_hp;

        } else if (isAPython<_PySequence_>( pyObj )) {
          return convert<_PySequence_,OT::HistogramPair>( pyObj );

        } else {
          throw OT::InvalidArgumentException(HERE) << "Object passed as argument is neither an HistogramPair nor an object convertible to an HistogramPair";
        }
        return OT::HistogramPair();
      }


  } /* namespace OT */

  %}

%include HistogramPair_doc.i

%include HistogramPair.hxx

namespace OT { %extend HistogramPair { HistogramPair(const HistogramPair & other) { return new OT::HistogramPair(other); } } }
