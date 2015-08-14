//                                               -*- C++ -*-
/**
 *  @brief An object that binds itself to an external shared library
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
#ifndef OPENTURNS_WRAPPEROBJECT_HXX
#define OPENTURNS_WRAPPEROBJECT_HXX

#include "PersistentObject.hxx"
#include "Library.hxx"
#include "WrapperData.hxx"
#include "NumericalPoint.hxx"
#include "NumericalSample.hxx"
#include "Field.hxx"
#include "Matrix.hxx"
#include "SymmetricTensor.hxx"
#include "Description.hxx"
#include "Pointer.hxx"

/* Forward declarations */
struct WrapperExchangedData;
struct WrapperError;
struct WrapperInformation;

BEGIN_NAMESPACE_OPENTURNS

/* Forward declaration */
struct WrapperSymbols;

/**
 * @class WrapperObject
 * @brief An object that binds itself to an external shared library
 *
 * A WrapperObject offers an interface for any object (function,
 * gradient, hessian) that is located in a external shared
 * library.
 */

class OT_API WrapperObject
  : public PersistentObject
{
  CLASSNAME;

public:

  // For test
  //static const UnsignedInteger NBCPUS_;


  enum ObjectType
  {
    FUNCTION = 0,
    GRADIENT,
    HESSIAN,
    ObjectPrefixSize
  };

  enum FunctionType
  {
    STATE_CREATION = 0,
    STATE_DELETION,
    INFORMATION,
    INITIALIZATION,
    EXECUTION,
    EXECUTION_SAMPLE,
    EXECUTION_FIELD,
    FINALIZATION,
    FunctionPrefixSize
  };

private:

  class ObjectPrefix : public std::vector<String>
  {
  public:
    ObjectPrefix();
  }; /* class ObjectPrefix */

  /** Array of prefix according to object type */
  static const ObjectPrefix ObjectPrefix_;

  class FunctionPrefix : public std::vector<String>
  {
  public:
    FunctionPrefix();
  }; /* class FunctionPrefix */

  /** Array of prefix according to function type */
  static const FunctionPrefix FunctionPrefix_;

  class Error
  {
    Pointer<struct WrapperError> p_error_;
  public :
    Error();
    ~Error();
    const struct WrapperError * get() const;
    struct WrapperError * get();
  };

public:

  /**
   * Constructor
   *
   * @param libraryPath the path of the library to load
   * @param symbolName  the name of the symbol to bind to
   * @param data        the data to send to the wrapper
   * @param o           the kind of this object
   */
  WrapperObject(const FileName    & libraryPath,
                const String      & symbolName,
                const WrapperData & data,
                ObjectType    o);

  /** Destructor */
  ~WrapperObject();

  /** Copy constructor */
  WrapperObject(const WrapperObject & other);

  /* Virtual constructor */
  virtual WrapperObject * clone() const;

  /* Comparison operator */
  Bool operator ==(const WrapperObject & other) const;

  /* String converter */
  virtual String __repr__() const;
  String __str__(const String & offset = "") const;

  /**
   * @brief Returns the name of the symbol in the shared library.
   * @param f the kind of function we want to bind to
   * @return The name of the symbol in the library
   * @see Library::getSymbol(String) const
   */
  String getFunctionName(FunctionType f) const;


  /** Returns the dimension of the input point.
   * @param p_state An hidden opaque pointer that points to the storage allocated by the wrapper as an internal state.
   * @throw WrapperInternalException
   */
  UnsignedInteger getInNumericalPointDimension(void * p_state) const;

  /** Returns the dimension of the output point.
   * @param p_state An hidden opaque pointer that points to the storage allocated by the wrapper as an internal state.
   * @throw WrapperInternalException
   */
  UnsignedInteger getOutNumericalPointDimension(void * p_state) const;

  /** Returns the description of the input and output variables of the functions.
   * @return The description of the input and output variables of the functions.
   * @throw WrapperInternalException
   */
  Description getDescription() const;

  /** @brief Initializes the wrapper in order to start further computations.
   *
   * The method is where all preliminary actions should be done before lauching the first computation:
   * creating directories, copying files, connecting to servers, etc.
   * @param p_state An hidden opaque pointer that points to the storage allocated by the wrapper as an internal state.
   * The meaning of this internal state is wrapper specific, so OpenTURNS doesn't need to know anything about it.
   * This internal state is allocated during the call to the createNewState method.
   * @throw WrapperInternalException
   * @see createNewState()
   */
  void initialize(void * p_state) const;

  /** @brief Realizes the desired computation within the wrapper.
   *
   * Initialization must have occured before the call to execute().
   * @param p_state The internal state
   * @param in A numerical point where the computation must be done
   * @return A numerical point which is the result of the computation
   * @throw InvalidArgumentException
   * @throw WrapperInternalException
   * @see createNewState()
   * @see initialize(void*) const
   */
  NumericalPoint execute(void * p_state, const NumericalPoint & inP) const;

  /** @brief Realizes the desired computation within the wrapper.
   *
   * Initialization must have occured before the call to execute().
   * This method acts as if the execute method was repeatedly called on the successive points of the sample,
   * but it may use (or not) some specific method of the wrapper to run the whole computation on the sample.
   * See func_exec_sample_ wrapper method for more detail.
   * @param p_state The internal state
   * @param in A numerical sample where the computation must be done
   * @return A numerical sample which is the result of the computation
   * @throw InvalidArgumentException
   * @throw WrapperInternalException
   * @see createNewState()
   * @see initialize(void*) const
   * @see execute(void*,const NumericalPoint&) const
   */
  NumericalSample execute(void * p_state, const NumericalSample & inS) const;

  /** @brief Realizes the desired computation within the wrapper.
   *
   * Initialization must have occured before the call to execute().
   * This method acts as if the execute method was repeatedly called on the successive points of the sample,
   * but it may use (or not) some specific method of the wrapper to run the whole computation on the sample.
   * See func_exec_field_ wrapper method for more detail.
   * @param p_state The internal state
   * @param in A field where the computation must be done
   * @return A field which is the result of the computation
   * @throw InvalidArgumentException
   * @throw WrapperInternalException
   * @see createNewState()
   * @see initialize(void*) const
   * @see execute(void*,const NumericalPoint&) const
   */
  Field execute(void * p_state, const Field & inField) const;

  /** @brief Realizes the computation of the gradient within the wrapper.
   *
   * Initialization must have occured before the call to gradient().
   * @param p_state The internal state
   * @param in A numerical point where the computation must be done
   * @return A matrix which is the result of the computation
   * @throw InvalidArgumentException
   * @throw WrapperInternalException
   * @see createNewState()
   * @see initialize(void*) const
   */
  Matrix gradient(void * p_state, const NumericalPoint & inP) const;

  /** @brief Realizes the computation of the hessian within the wrapper.
   *
   * Initialization must have occured before the call to hessian().
   * @param p_state The internal state
   * @param in A numerical point where the computation must be done
   * @return A tensor which is the result of the computation
   * @throw InvalidArgumentException
   * @throw WrapperInternalException
   * @see createNewState()
   * @see initialize(void*) const
   */
  SymmetricTensor hessian(void * p_state, const NumericalPoint & inP) const;

  /** @brief Finalizes the wrapper in order to stop computations.
   *
   * The method is where all ultimate actions should be done when all computations are done:
   * removing files or directories, closing connections, etc.
   * @param p_state An hidden opaque pointer that points to the storage allocated by the wrapper as an internal state.
   * The meaning of this internal state is wrapper specific, so OpenTURNS doesn't need to know anything about it.
   * This internal state is allocated during the call to the createNewState method.
   * @throw WrapperInternalException
   * @see createNewState()
   */
  void finalize(void * p_state) const;

  /** @brief Asks the wrapper to build a brand new internal state.
   *
   * The internal state is a heap allocated storage managed by the wrapper. OpenTURNS doesn't know anything
   * about it except that it must pass it to every wrapper function. If the wrapper has no use of an internal
   * state it may return a null pointer: this isn't an error and shouldn't be treated so.
   * The internal state must be deleted by a call to deleteState().
   * @return An hidden opaque pointer that points to the storage allocated by the wrapper as an internal state.
   * @throw WrapperInternalException
   * @see deleteState(void*)
   */
  void * createNewState();

  /** @brief Asks the wrapper to delete a previously allocated internal state.
   *
   * The wrapper is in charge of deleting the internal state it allocated before. Don't delete it by yourself !
   * @param p_state An hidden opaque pointer that points to the storage allocated by the wrapper as an internal state.
   * @throw WrapperInternalException
   * @see createNewState()
   */
  void deleteState(void * p_state);


protected:

  /** Symbol accessor */
  LibrarySymbol getSymbol(const String & name, Bool optional = false, LibrarySymbol alternateSymbol = 0) const;

private:

  /** The library handle */
  Library handle_;

  /** The data send to the wrapper */
  WrapperData data_;
  struct WrapperExchangedData * p_exchangedData_;

  /** The kind of object we are */
  ObjectType type_;

  /** The error emited by the wrapper */
  mutable Error error_;

  /** The pointer to the library symbols structure */
  Pointer<struct WrapperSymbols> wrapperSymbols_;

  /* The information structure */
  mutable Pointer<struct WrapperInformation> wrapperInfo_;

}; /* class WrapperObject */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WRAPPEROBJECT_HXX */
