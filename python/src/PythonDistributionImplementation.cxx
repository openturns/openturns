//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' Distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "Python.h"
#include "swig_runtime.hxx"

#include "PythonDistributionImplementation.hxx"
#include "OSS.hxx"
#include "Description.hxx"
#include "PythonWrappingFunctions.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(PythonDistributionImplementation);

static const Factory<PythonDistributionImplementation> RegisteredFactory;



/* Default constructor */
PythonDistributionImplementation::PythonDistributionImplementation()
  : DistributionImplementation(),
    pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonDistributionImplementation::PythonDistributionImplementation(PyObject * pyObject)
  : DistributionImplementation(),
    pyObj_(pyObject)
{
  Py_XINCREF( pyObj_ );

  if ( !PyObject_HasAttrString( pyObj_, const_cast<char *>("computeCDF") ) ) throw InvalidArgumentException(HERE) << "Error: the given object does not have a computeCDF() method.";

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString ( pyObj_,
                            const_cast<char *>( "__class__" ) ) );
  ScopedPyObjectPointer name(PyObject_GetAttrString( cls.get(),
                             const_cast<char *>( "__name__" ) ) );

  setName( checkAndConvert<_PyString_, String>(name.get()) );

  // Set dimension
  ScopedPyObjectPointer dim(PyObject_CallMethod ( pyObj_,
                            const_cast<char *>( "getDimension" ),
                            const_cast<char *>( "()" ) ));
  setDimension(checkAndConvert< _PyInt_, UnsignedInteger >( dim.get() ));

  if ( !PyObject_HasAttrString( pyObj_, const_cast<char *>("computeCDF") ) ) throw InvalidArgumentException(HERE) << "Error: the given object does not have a computeCDF() method.";

  if ((getDimension() > 1) && !PyObject_HasAttrString( pyObj_, const_cast<char *>("getRange") ) ) throw InvalidArgumentException(HERE) << "Error: the given object does not have a getRange() method.";

  computeRange();
}

/* Virtual constructor */
PythonDistributionImplementation * PythonDistributionImplementation::clone() const
{
  return new PythonDistributionImplementation(*this);
}

/* Copy constructor */
PythonDistributionImplementation::PythonDistributionImplementation(const PythonDistributionImplementation & other)
  : DistributionImplementation(other),
    pyObj_(other.pyObj_)
{
  Py_XINCREF( pyObj_ );
}

/* Destructor */
PythonDistributionImplementation::~PythonDistributionImplementation()
{
  Py_XDECREF( pyObj_ );
}

/* Comparison operator */
Bool PythonDistributionImplementation::operator ==(const PythonDistributionImplementation & other) const
{
  return true;
}

/* String converter */
String PythonDistributionImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonDistributionImplementation::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription();
  return oss;
}

/* String converter */
String PythonDistributionImplementation::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << PythonDistributionImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}


/* Here is the interface that all derived class must implement */

NumericalPoint PythonDistributionImplementation::getRealization() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getRealization") ) )
  {


    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj_,
                                     const_cast<char *>( "getRealization" ),
                                     const_cast<char *>( "()" ) ) );
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Realization returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getRealization();
  }
}

/* Numerical sample accessor */
NumericalSample PythonDistributionImplementation::getSample(const UnsignedInteger size) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getSample")))
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_ >("getSample"));
    ScopedPyObjectPointer sizeArg(convert< UnsignedInteger, _PyInt_ >( size ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     sizeArg.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalSample result(convert<_PySequence_, NumericalSample>(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Sample returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    if (result.getSize() != size) throw InvalidDimensionException(HERE) << "Sample returned by PythonDistribution has incorrect size. Got " << result.getSize() << ". Expected" << size;
    return result;
  }
  else
  {
    return DistributionImplementation::getSample(size);
  }
}


/* Get the DDF of the distribution */
NumericalPoint PythonDistributionImplementation::computeDDF(const NumericalPoint & inP) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeDDF")))
  {
    const UnsignedInteger dimension = inP.getDimension();
    if (dimension != getDimension())
      throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "computeDDF" ));
    ScopedPyObjectPointer point(convert<NumericalPoint, _PySequence_>(inP));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     point.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != dimension) throw InvalidDimensionException(HERE) << "DDF returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << dimension;
    return result;
  }
  else
  {
    return DistributionImplementation::computeDDF(inP);
  }
}

/* Get the PDF of the distribution */
NumericalScalar PythonDistributionImplementation::computePDF(const NumericalPoint & inP) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computePDF")))
  {
    const UnsignedInteger dimension = inP.getDimension();
    if (dimension != getDimension())
      throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>("computePDF"));
    ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >(inP));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     point.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalScalar result = convert< _PyFloat_, NumericalScalar >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::computePDF(inP);
  }
}

/* Get the PDF of the distribution */
NumericalScalar PythonDistributionImplementation::computeLogPDF(const NumericalPoint & inP) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeLogPDF")))
  {
    const UnsignedInteger dimension = inP.getDimension();
    if (dimension != getDimension())
      throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "computeLogPDF" ));
    ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >( inP ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     point.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalScalar result = convert< _PyFloat_, NumericalScalar >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::computeLogPDF(inP);
  }
}

/* Get the CDF of the distribution */
NumericalScalar PythonDistributionImplementation::computeCDF(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension = inP.getDimension();
  if (dimension != getDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
  ScopedPyObjectPointer methodName(convert< String, _PyString_>("computeCDF"));
  ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >(inP));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                   methodName.get(),
                                   point.get(), NULL));
  if (callResult.isNull())
  {
    handleException();
  }
  NumericalScalar result = convert< _PyFloat_, NumericalScalar >(callResult.get());
  return result;
}


/* Get the complementary CDF of the distribution */
NumericalScalar PythonDistributionImplementation::computeComplementaryCDF(const NumericalPoint & inP) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeComplementaryCDF") ) )
  {
    const UnsignedInteger dimension = inP.getDimension();
    if (dimension != getDimension())
      throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "computeComplementaryCDF" ));
    ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >( inP ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs ( pyObj_,
                                     methodName.get(),
                                     point.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalScalar result = convert< _PyFloat_, NumericalScalar >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::computeComplementaryCDF(inP);
  }
}


/* Get the PDFGradient of the distribution */
NumericalPoint PythonDistributionImplementation::computeQuantile(const NumericalScalar prob, const Bool tail) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeQuantile")))
  {
    const UnsignedInteger dimension = getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>("computeQuantile"));
    ScopedPyObjectPointer probArg(convert< NumericalScalar, _PyFloat_ >(prob));
    ScopedPyObjectPointer tailArg(convert< Bool, _PyBool_ >(tail));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     probArg.get(), tailArg.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != dimension) throw InvalidDimensionException(HERE) << "Quantile returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << dimension;
    return result;
  }
  else
  {
    return DistributionImplementation::computeQuantile(prob, tail);
  }
}


/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex PythonDistributionImplementation::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeCharacteristicFunction")))
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_>("computeCharacteristicFunction"));
    ScopedPyObjectPointer cX(convert< NumericalScalar, _PyFloat_ >( x ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     cX.get(), NULL));

    if (callResult.isNull())
    {
      handleException();
    }
    NumericalComplex result = convert< _PyComplex_, NumericalComplex >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::computeCharacteristicFunction( x );
  }
}

/* Get the PDFGradient of the distribution */
NumericalPoint PythonDistributionImplementation::computePDFGradient(const NumericalPoint & inP) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computePDFGradient")))
  {
    const UnsignedInteger dimension = inP.getDimension();
    if (dimension != getDimension())
      throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>("computePDFGradient"));
    ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >(inP));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     point.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != dimension) throw InvalidDimensionException(HERE) << "PDFGradient returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << dimension;
    return result;
  }
  else
  {
    return DistributionImplementation::computePDFGradient(inP);
  }
}

/* Get the CDFGradient of the distribution */
NumericalPoint PythonDistributionImplementation::computeCDFGradient(const NumericalPoint & inP) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeCDFGradient")))
  {
    const UnsignedInteger dimension = inP.getDimension();
    if (dimension != getDimension())
      throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getDimension();
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "computeCDFGradient"));
    ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >(inP));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     point.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != dimension) throw InvalidDimensionException(HERE) << "CDFGradient returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << dimension;
    return result;
  }
  else
  {
    return DistributionImplementation::computeCDFGradient(inP);
  }
}

/* Get the quantile of the distribution */
NumericalScalar PythonDistributionImplementation::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("computeScalarQuantile") ) )
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_>("computeScalarQuantile"));
    ScopedPyObjectPointer cProb(convert< NumericalScalar, _PyFloat_ >( tail ? 1 - prob : prob ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     cProb.get(), NULL));

    if (callResult.isNull())
    {
      handleException();
    }
    NumericalScalar result = convert< _PyFloat_, NumericalScalar >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::computeScalarQuantile(prob, tail);
  }
}


/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar PythonDistributionImplementation::getRoughness() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getMean")))
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "getRoughness" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalScalar result = convert< _PyFloat_, NumericalScalar >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::getRoughness();
  }
}

/* Mean accessor */
NumericalPoint PythonDistributionImplementation::getMean() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getMean")))
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "getMean" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Mean returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getMean();
  }
}

/* Standard deviation accessor */
NumericalPoint PythonDistributionImplementation::getStandardDeviation() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getStandardDeviation") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "getStandardDeviation" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Standard deviation returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getStandardDeviation();
  }
}

/* Skewness accessor */
NumericalPoint PythonDistributionImplementation::getSkewness() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getSkewness") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "getSkewness" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Skewness returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getSkewness();
  }
}

/* Kurtosis accessor */
NumericalPoint PythonDistributionImplementation::getKurtosis() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getKurtosis") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "getKurtosis" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Kurtosis returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getKurtosis();
  }
}


/* Get the raw moments of the distribution */
NumericalPoint PythonDistributionImplementation::getStandardMoment(const UnsignedInteger n) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getStandardMoment") ) )
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "getStandardMoment" ));
    ScopedPyObjectPointer nArg(convert< UnsignedInteger, _PyInt_ >( n ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     nArg.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Moment returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getStandardMoment( n );
  }
}


/* Get the raw moments of the distribution */
NumericalPoint PythonDistributionImplementation::getMoment(const UnsignedInteger n) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getMoment") ) )
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "getMoment" ));
    ScopedPyObjectPointer nArg(convert< UnsignedInteger, _PyInt_ >( n ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     nArg.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Moment returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getMoment( n );
  }
}

/* Get the centered moments of the distribution */
NumericalPoint PythonDistributionImplementation::getCenteredMoment(const UnsignedInteger n) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getCenteredMoment") ) )
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "getCenteredMoment" ));
    ScopedPyObjectPointer nArg(convert< UnsignedInteger, _PyInt_ >( n ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     nArg.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    NumericalPoint result(convert< _PySequence_, NumericalPoint >(callResult.get()));
    if (result.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Centered moment returned by PythonDistribution has incorrect dimension. Got " << result.getDimension() << ". Expected" << getDimension();
    return result;
  }
  else
  {
    return DistributionImplementation::getCenteredMoment( n );
  }
}


/* Check if the distribution is a copula */
Bool PythonDistributionImplementation::isCopula() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("isCopula") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "isCopula" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Bool result = convert< _PyBool_, Bool >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::isCopula();
  }
}


/* Check if the distribution is elliptical */
Bool PythonDistributionImplementation::isElliptical() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("isElliptical") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "isElliptical" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Bool result = convert< _PyBool_, Bool >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::isElliptical();
  }
}


/* Check if the distribution is continuous */
Bool PythonDistributionImplementation::isContinuous() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("isContinuous") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj_,
                                     const_cast<char *>( "isContinuous" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Bool result = convert< _PyBool_, Bool >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::isContinuous();
  }
}


/* Check if the distribution is integral */
Bool PythonDistributionImplementation::isIntegral() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("isIntegral") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "isIntegral" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Bool result = convert< _PyBool_, Bool >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::isIntegral();
  }
}


/* Tell if the distribution has elliptical copula */
Bool PythonDistributionImplementation::hasEllipticalCopula() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("hasEllipticalCopula") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "hasEllipticalCopula" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Bool result = convert< _PyBool_, Bool >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::hasEllipticalCopula();
  }
}


/* Tell if the distribution has independent copula */
Bool PythonDistributionImplementation::hasIndependentCopula() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("hasIndependentCopula") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod ( pyObj_,
                                     const_cast<char *>( "hasIndependentCopula" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Bool result = convert< _PyBool_, Bool >(callResult.get());
    return result;
  }
  else
  {
    return DistributionImplementation::hasIndependentCopula();
  }
}


/* Get the distribution of the marginal distribution corresponding to indices dimensions */
PythonDistributionImplementation::Implementation PythonDistributionImplementation::getMarginal(const Indices & indices) const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getMarginal")))
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_ >("getMarginal"));
    ScopedPyObjectPointer indicesArg(convert< Indices, _PySequence_ >(indices));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_,
                                     methodName.get(),
                                     indicesArg.get(), NULL));
    if (callResult.isNull())
    {
      handleException();
    }
    void * ptr = 0;
    OT::Distribution * p_outDist = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(callResult.get(), &ptr, SWIG_TypeQuery("OT::Distribution *"), 0)))
    {
      p_outDist = reinterpret_cast< OT::Distribution * >(ptr);
    }
    else
    {
      throw InvalidArgumentException(HERE) << "Output value for " << getName() << ".getMarginal method is not a Distribution";
    }
    return p_outDist->getImplementation();
  }
  else
  {
    return DistributionImplementation::getMarginal(indices);
  }
}

/* Method save() stores the object through the StorageManager */
void PythonDistributionImplementation::save(Advocate & adv) const
{
  DistributionImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonDistributionImplementation::load(Advocate & adv)
{
  DistributionImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}



template <>
inline
Interval
convert< _PyObject_, Interval >(PyObject * pyObj)
{
  NumericalPoint lowerBound;
  if ( PyObject_HasAttrString( pyObj, const_cast<char *>("getLowerBound") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj,
                                     const_cast<char *>( "getLowerBound" ),
                                     const_cast<char *>( "()" ) ));
    check< _PySequence_ >(callResult.get());
    if (callResult.isNull())
    {
      handleException();
    }
    lowerBound = convert< _PySequence_, NumericalPoint >(callResult.get());
  }

  NumericalPoint upperBound;
  if ( PyObject_HasAttrString( pyObj, const_cast<char *>("getUpperBound") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj,
                                     const_cast<char *>( "getUpperBound" ),
                                     const_cast<char *>( "()" ) ));
    check< _PySequence_ >(callResult.get());
    if (callResult.isNull())
    {
      handleException();
    }
    upperBound = convert< _PySequence_, NumericalPoint >(callResult.get());
  }

  Indices finiteLowerBound;
  if ( PyObject_HasAttrString( pyObj, const_cast<char *>("getFiniteLowerBound") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj,
                                     const_cast<char *>( "getFiniteLowerBound" ),
                                     const_cast<char *>( "()" ) ));
    check< _PySequence_ >(callResult.get());
    if (callResult.isNull())
    {
      handleException();
    }
    finiteLowerBound = convert< _PySequence_, Indices >(callResult.get());
  }

  Indices finiteUpperBound;
  if ( PyObject_HasAttrString( pyObj, const_cast<char *>("getFiniteUpperBound") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj,
                                     const_cast<char *>( "getFiniteUpperBound" ),
                                     const_cast<char *>( "()" ) ));
    check< _PySequence_ >(callResult.get());
    if (callResult.isNull())
    {
      handleException();
    }
    finiteUpperBound = convert< _PySequence_, Indices >(callResult.get());
  }

  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}





/* Compute the numerical range of the distribution given the parameters values */
void PythonDistributionImplementation::computeRange()
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("getRange") ) )
  {
    ScopedPyObjectPointer callResult(PyObject_CallMethod( pyObj_,
                                     const_cast<char *>( "getRange" ),
                                     const_cast<char *>( "()" ) ));
    if (callResult.isNull())
    {
      handleException();
    }
    Interval range(convert< _PyObject_, Interval >(callResult.get()));
    setRange(range);
  }
  else
  {
    DistributionImplementation::computeRange();
  }
}

END_NAMESPACE_OPENTURNS
