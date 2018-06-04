Coding rules
============

Packages
--------

In order to structure the code of the  project, the various elements
(classes, functions, libraries, data) are logically organized in
packages. This chapter describes the rules to be followed for the
definition, management and use of these packages.

| The code is mainly located in a single library. This library is
  organized as a set of modules. However, there may be several
  interacting libraries in the future.
| The library is interfaced with Python through a Python module
  exposing almost all the classes and operators.

For the moment, the entire set of classes is located in **libOT.so** for
the dynamic part and in **libOT.a** for the static part.

*Example showing the import of modules via the **openturns** Python
package*

::

    import openturns
    import openturns.base
    import openturns.uncertainty

*Example showing the direct import of module operators or classes via
the **openturns** Python package*

::

    from openturns import Point
    from openturns.base import Sample
    from openturns.uncertainty import RandomVector

File names
----------

The definition of filenames obeys a few rules described below, according
to the programming languages used. A general rule is preliminarily
defined in order to facilitate the automatic generation of the
**Makefile** files. The file names consist of sequences of characters
separated by a dot. The first part of the name is called the *base* and
the second is called the *suffix* (or *extension*).

+---------------------+-----------------------------------------------------------------------------------------------------------+
| **Extension**       | **Description**                                                                                           |
+=====================+===========================================================================================================+
| **.hxx .hh .hpp**   | Header file containing the declaration of functions and classes and the definition of templates for C++   |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.cxx .cc .cpp**   | Source code file containing the definition (implementation) of C++ functions and classes                  |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.c**              | Source code file containing the definition of C functions                                                 |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.h**              | Header file containing the declaration of functions in C                                                  |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.py**             | Python file                                                                                               |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.R**              | R file                                                                                                    |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.cmake**          | CMake script                                                                                              |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.sh**             | Shell script                                                                                              |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.bat**            | DOS script                                                                                                |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.conf**           | configuration file                                                                                        |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.csv**            | Comma Separated Value file (for samples)                                                                  |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.i**              | SWIG interface file                                                                                       |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.in**             | Template file                                                                                             |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.log**            | Output log file                                                                                           |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.mws**            | Maple script file                                                                                         |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.nsi**            | Windows installer file                                                                                    |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.sce**            | Scilab script file                                                                                        |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.a**              | Archive file containing statically linked objects                                                         |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.so**             | Shared object file containing dynamically linked objects                                                  |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.txt**            | Text file                                                                                                 |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.xml**            | XML file (mainly for wrapper description file)                                                            |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.ll**             | Lex scanner file                                                                                          |
+---------------------+-----------------------------------------------------------------------------------------------------------+
| **.yy**             | Yacc parser file                                                                                          |
+---------------------+-----------------------------------------------------------------------------------------------------------+

*For example, it is not recommended to give the following names to two
files in the same directory:*

::

    matrix.cxx
    Matrix.cxx

C++ Files
~~~~~~~~~

*Example: one file per class:*

::

    Sample.hxx declares class Sample
    Matrix.hxx declares class Matrix
    ...

*Incorrect example: one file for all classes of a model:*

::

    Model.hxx # contains all the declaration of all the classes of the internal model

The preceding rule has one exception: in order to facilitate the use of
several related classes, the header files belonging to the same module
are grouped in a single header file, which bears the same name as the
module and is prefixed by **OT**.

*Example: using all the classes of the Base module:*

::

    #include "openturns/OTBase.hxx"

Header files
~~~~~~~~~~~~

The header files are used to declare functions and classes (they are
sometimes called *interface definition* or *interface specification*).

*Example for a file named **Sample.hxx***

::

    #ifndef OPENTURNS_SAMPLE_HXX
    #define OPENTURNS_SAMPLE_HXX
    ...
    #endif /* OPENTURNS_SAMPLE_HXX */

*Example of header file inclusion*

::

    #include "openturns/OSS.hxx"
    #include "openturns/Point.hxx"

*Example for the inclusion of system function or external library header
files*

::

    #include <cstring>
    #include <boost/python.hpp>

*Example for the inclusion of non standard system function header files*

::

    extern "C" (
    #include <nonstandard.h>
   )

Test files
~~~~~~~~~~

*Example of names for test files*

::

    t_Matrix_construction.cxx
    t_Matrix_assignment.cxx
    t_Matrix_bug7654.cxx
    t_Matrix_vectorMultiplication.cxx

C++
---

This section deals with coding rules for the C++ language.

Compilation flags
~~~~~~~~~~~~~~~~~

It is helpful to enable some compilation warnings to avoid questionable constructions.
You may also want to enable debug symbols for further use with a debugger.

*GCC compilation:*

::

    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wno-unused-parameter -D_GLIBCXX_ASSERTIONS" ..
    make

Namespaces
~~~~~~~~~~

*Example of **OpenTURNS** namespace definition for simple types:*

::

    / **
    * @file       OTtypes.hxx
    * ...
    * /
    namespace OT
    {
    /* < Declarations of simple types > * /

    /* < Declarations of objects and functions > * /
    };

    // Alias for the direct use of XXX
    namespace OpenTURNS = OT;

*Example of use by making all the definitions contained in the namespace
available:*

::

    #include "openturns/OT.hxx"
    using namespace OT;

    void f(Scalar n);

Names
~~~~~
Names of classes, variables and methods consist of concatenated ful words.
Each word begins with an uppercase, except for the first one.
The first word begins with a lowercase except for class names and static methods or variables.
No abbreviations are allowed, except if it is found in the literature, for example FORM.


*Example:*

::

    class Sample {
    ...
    }; /* end class Sample */

*Example:*

::

    int main() {
    Bool myCondition = false;
    ...
    }

*Example:*

::

    class Environment : public Object {
    ...
    private:
    Scalar density_; //<! material density in environment (g/cm3)
    ...
    }; /* end class Environment */

NB: It is common for the underscore to be used as a prefix for private
attribute names. However, this method may trigger conflicts with
internal variables or definitions used by the compilers. For this
reason, the underscore is used as a suffix.

*Example:*

::

    class Object {
    ...
    private:
    static String ClassName_;
    ...
    }; /* end class Object */

*Example:*

::

    class Object
    {
    public:
      //* returns a class identifier based on its name
      static  String GetClassName(); ...
    }; /* end class Object */

*Example:*

::

    int
    initializeConversion()
    {
      static Bool IsInitialized = false;
      if (! IsInitialized) {
        ...
        IsInitialized = true;
      }
    };

*Example:*

::

    const UnsignedInteger MaximumOfRetries = 5;

*Example:*

::

    int main()
    {
      Scalar reactionRate = 0.0;
      ...
    }

*Example:*

::

    class Sample
    {
      UnsignedInteger getDimension () const;
      ...
    }; /* end class Sample */

*Example:*

::

    class Sample {
    }; /* end class Sample */

    void removeElement(const UnsignedInteger index);

    Point meanValue;

*Example of tolerated notations:*

::

    UnsignedInteger i;                // loop indices i, j and k are common
    UnsignedInteger j;
    UnsignedInteger k;

    UnsignedInteger nbMaxElements;    // usual abbreviations: nb, Max

    void
    addPoint(Point pt) { // no confusion in the context
      add(pt);
    }

*Incorrect examples:*

::

    Scalar a, k, l, m1, m2, m3;
    Scalar zzz, zz2;
    const char *foo, *hello, tempo, bogus;

    void adElt(Point pt);

    UnsignedInteger numSmplPt;

Class declaration
~~~~~~~~~~~~~~~~~

*Example:*

::

    class Buffer {
    public :
      static AThing GetThing();
    protected:
    private :
      static AThing TheThing_;

    public :
      Scalar getValue() const;
    protected :
      Scalar theValue_;
    private :
      /* ... */
    }; /* end class Buffer */

*Example:*

::

    class AnyClass {
    public :
      /** Default constructor  */
      AnyClass();
      /** Copy constructor */
      AnyClass(const AnyClass & other);
      /** Destructor */
      virtual ~AnyClass();
      /** Copy operator  */
      AnyClass& operator = (const AnyClass & other);
      /** Comparison operator */
      Bool operator == (const AnyClass & other) const;
      /** Stream converter */
      String repr() const;
      String str() const;

      /* other public methods ... */

    private :
      UnsignedInteger size_;
      DataType * data_;

      /* other private methods ... */
    }; /* end class AnyClass */

*Example:*

::

    class AnyClass {
    public :
      /* ... */
    private :
      UnsignedInteger size_;
      DataType * data_;
    }; /* end class AnyClass */

*Example:*

::

    class Vector {
    public :
      Vector (Bool someProperty, UnsignedInteger size, Scalar elt = 0.);
    private :
      Bool property_;
      Collection<Scalar> data_;
    };

*Example of a correct definition:*

::

    Vector::Vector (Bool someProperty, UnsignedInteger size, Scalar elt)
    : property_(someProperty)
    , data_(size, elt)
    { }

*Examples of incorrect definitions:*

::

    Vector::Vector (Bool someProperty, UnsignedInteger size, Scalar elt)
    : data_(size, elt)
    , property_(someProperty)     // order of initialization
    { }

    Vector::Vector (Bool someProperty, UnsignedInteger size, Scalar elt)
    {
      property_ = someProperty;
      data_ = Collection<Scalar>(size, elt);
      // requires an assignment after the construction
      // processing is longer for complex objects!
    }

*Example: declaration of a pure virtual class A and of class B, derived
from A:*

::

    class A {
    public :
      A();                  // constructor
      virtual ~A();          // destructor
      virtual const char * getClassName() = 0;       // pure virtual method
    };

    class B : public A {
    public :
      const char * getClassName() { return "B"; }
    };

*Incorrect definitions leading to an execution error:*

::

    A::A() {
    cout << getClassName() << " created" << endl; // B does not exist yet!
    }

    A::~A() {
    cout << getClassName() << " destroyed" << endl; // B no longer exists!
    }

    B::B() : A()
    { }

*Write method for the **name** attribute:*

::

    void            setName (SimpleType);
    void setName    (const ComposedType &);

*Read method for the **name** attribute:*

::

    SimpleType              getName() const;
    const ComposedType &    getName() const;

*Example:*

::

    class Sample {
    public :
      //* return the dimension of the sample
      UnsignedInteger getDimension() const;

      //* return the i-th element
      Point         operator[] (UnsignedInteger i);
      const Point & operator[] (UnsignedInteger i) const;
    };

*Example:*

::

    class Sample {
    public :
      //* return the number of the rod
      inline UnsignedInteger getDimension() const { return dimension_; }

      //* compute the mean point of the sample
      inline Point computeMeanValue() const;
    };

    //* inline methods
    Point computeMeanValue() const;
    {
    /* ... some non trivial processing */
    return meanValue;
    }

Explicit keyword
~~~~~~~~~~~~~~~~

Marking a single argument class constructors with the the *explicit* keyword
allows to avoid unwanted conversions.

It is relevant for constructors that have a single-argument, and also for
constructors that have a single mandatory argument plus optional arguments.

*Single argument:*

::

    class A {
    public :
      explicit A(const Point value);
    };

*Optional argument:*

::

    class A {
    public :
      explicit A(UnsignedInteger max = 6);
    };

*Mandatory argument and optional argument:*

::

    class A {
    public :
      explicit A(const Point value, UnsignedInteger max = 6);
    };

Inheritance
~~~~~~~~~~~

*Example: the Point class derives from the Vector class*

::

    class Point : public std::vector<double> {
    public:
      Point(Scalar x,
            Scalar y,
            Scalar z);
    };

    Point::Point(Scalar x, Scalar y,
    Scalar z)
    : std::vector<double>(3)
    {
      (*this)[0] = x;
      (*this)[1] = y;
      (*this)[2] = z;
    }

*Example:*

::

    class Object {
    public :
      Object();
      virtual ~Object();
    };

Function and method declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    /** @brief <short description>
    *
    * <Long description>
    * @param argument_1 <description>
    * @param argument_2 <description>
    * @return           <description>
    * @throw            <description>
    */
    ReturnType
    functionName (
    TypeArgument_1       argument_1,
    TypeArgument_2   argument_2
   );

*Correct example:*

::

    void send(const String & message);

*Incorrect Example:*

::

    void send(String message);

*Correct example:*

::

    Buffer & append(UnsignedInteger);
    Buffer & append(const String &);
    Buffer & append(Scalar);

*Incorrect Example:*

::

    Buffer & append(const char *fmt, ...);
    Buffer & append(const char *str = 0, double d = 0., int i = 0);

Variable declaration
~~~~~~~~~~~~~~~~~~~~
An atomic variable type (integer, bool, pointer, ...) must be always
initialized to a value to avoid undefined behavior.
This includes initialization of class attributes.


*Correct example:*

::

    String         filename (""); // library file name
    UnsignedInteger nbElements = 0; // number of elements into the data file
    UnsignedInteger i = 0;
    Scalar f = 0.0;

*Accepted example:*

::

    UnsignedInteger i = 0, j = 0, k = 0;     // indices

*Incorrect Example:*

::

    / * Multiple declarations and different types * /
    UnsignedInteger   i, j, tab[20], **l, *numberOfElements;
    String        filename, *yourname, myname;

Constant declaration
~~~~~~~~~~~~~~~~~~~~
The const keyword must be used as much as possible.
Float constants must include the decimal separator and a at least a digit to
explicitely distinguish them from integers.

*Example:*

::

    const Scalar f = 6.0;
    const UnsignedInteger maximumIterations = 32;
    const char printFormat[] = "%s:line %d, %s";

*Incorrect Example:*

::

    #define MAXIMUM_ITERATIONS 32;
    #define PRINT_FORMAT       "%s:line %d, %s"

Comments and internal documentation 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    /**
    * @brief short description
    *
    * <LGPL copyright>
    *
    * Copyright 2005-20YY Airbus-EDF-IMACS-Phimeca
    */

These comments should avoid:

-  mentioning the names of variables;

-  being a simple transcription of the code into English.

Memory allocation and deallocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section discusses general rules for allocating and freeing memory.
It will later be supplemented by rules regarding the use of basic
classes in order to manage the lifecycle of objects in memory.

*Example to favor:*

::

    {
      Scalar sections1[MAX]; // a fixed size array
      vector<Scalar> sections2; // an extensible vector
      list<Volume> volumes; // a list of volumes

      /* ... */
    }

*Example to avoid:*

::

    {
      Scalar *sections = new Scalar[MAX];
      list<Volume>    *volumes  = new list<Volume>;

      /* ... */

      delete [ ] sections;
      delete volumes;
    }

*Correct example:*

::

    {
      Volume *volume = new Volume;   // memory allocation +
      // constructor call
      /* ... */
      delete volume;                 // destructor call +
      volume = 0;                    // memory deallocation
    }

*Incorrect example:*

::

    {
      Volume *volume = (Volume*)malloc(sizeof(Volume));
      // memory allocation but
      // no constructor call
      /* ... */
      free(volume);                // no destructor call before
      volume = 0;                    // memory deallocation
    }

*Example:*

::

    A *     a = new A[40]; // calls the constructor 40 times
    ...

::

    delete a;              // incorrect: the table is freed,
    // the ~A destructor isn't called

::

    delete [] a;           // correct: the table is freed,
    // the ~A destructor is called 40 times

*List of declaration files for the smart pointer:*

::

    #include "openturns/Pointer.hxx"

Assignment and initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Complex types (class types) must use copy constructors if available instead of
using the default constructor and then the copy operator for performance.
Atomic types (integer, bool, ...) can use the copy operator for readability.

*Example:*

::

    Point p2(p1);
    Bool a = b;

*Example to avoid:*

::

    String message;
    message = "Computation complete"; // assignment after construction

    String message(); // declaration of a function prorotype

Instructions
~~~~~~~~~~~~

*Example:*

::

    i = 0;
    while (i < MAX) {
      ++i;
      f(i);
    }

*Examples to avoid if possible:*

::

    a = b = c = 0;
    // multiple assignments

    f(++i);
    // readability

    v = *i++;
    // performance and understandability

    for(i = 1, j = 2, k = 3; i < N; j++, i++);
    // understandability and readability

*Incorrect examples:*

::

    buffer += "test", cout << buffer; i = 1;
    // heterogeneous processing &
    // different objects

    while(f(++i), i < MAX);
    // processing carried out before the test

*Prohibited example:*

::

    void foo() {
      for(...) {
        phase1 :
        /* ... */
        phase2 :
        if(errno != 0)
          goto erreur;
        if (/* a test */)
          goto phase2;
      }
      erreur :
      /* error handling */
    }

*Note: error handling can be easily replaced with an exception handling,
and the use of **goto** for the needs of algorithms can always be
replaced with a conditional structure or a loop.*

*Example:*

::

    Scalar
    compute(UnsignedInteger n) {
      Scalar result;
      if(n < MIN || n > MAX) {
        char msg[BUFSIZ];
        // automatic allocation for the processing
        sprintf (msg,
        "n = %d is out of range, valid range is [%d, %d]",
        n, MIN, MAX);
        throw Exception(msg);
      }
      /* ... */
      return result;
    }

*Examples to avoid:*

::

    Scalar
    compute(UnsignedInteger n) {
      Scalar result;
      Char    msg[BUFSIF];   // allocation unnecessary if no
      // error
      if(n < MIN || n > MAX)
      ...
    }

*Correct example:*

::

    switch (errno) {
    case ENOENT :
    msg = " ... ";

::

    break;
    case EACCESS :
    msg = " ... ";
    break;
    default :
    msg = "unknown error";
    break;
    }

*Accepted example - processing multiple targets with the same block:*

::

    switch (errno) {
    case ENOENT :
    case EACCESS :
    msg = " impossible to access file ";
    break;
    default :
    /* ... */
    break;
    }

*Incorrect example:*

::

    switch (errno) {
    case 1 :                // it is a value
    msg = " ... "; //
    // "break" is missing,
    // processing continues in ENOENT
    case ENOENT :
    msg = " ... ";
    break;
    default :               // "break" is missing at the
    // end of the "default" case
    msg = "unknown error";
    }

*Incorrect example - use of the switch as a goto:*

::

    switch (phase) {
    case PHASE1 :
    doPhaseOne();
    case PHASE2 :
    doPhaseTwo();
    break;
    default :
    /* ... */
    }

*Example:*

::

    int
    main (int argc, char *argv[])
    {
    /* ... */
    return EXIT_SUCCESS;
    }

Exceptions
~~~~~~~~~~

The ability to raise and handle exceptions is one of the strongest
features of C++. However, writing functions and methods that guarantee a
safe behavior when faced with exceptions remains a difficult aspect of
programming.

This chapter describes how to define and use exceptions in the source
code.

*Example of **Exception** use*

::

    class Exception {
    public :
      Exception (const char *description, const char * comment = 0);
      virtual ~Exception() throw();
      /* ... */
      friend ostream & operator<< (ostream &, const Exception & e);
    };

*Example of specialization of **Exception** in order to report an
off-range error*

::

    class OutOfBoundException : public Exception {
    public:
      OutOfBoundException(/* ... */)
      : Exception(/* ... */) { }
    };

*Example of specialization of **Exception** in order to report an
off-range error with a macro-instruction*

::

    NEW_EXCEPTION(OutOfBoundException);

*Incorrect Example:*

::

    try {
    // phase 1
    // phase 2
    if (result != OK)
    throw GotoPhase4();
    // phase 3

::

    /* ... */
    catch (GotoPhase4 e) {
    /* ... */
    }
    // phase 4

Error handling and error messages
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*Example:*

::

    String message;
    Log::Debug(message);
    Log::Info(message);
    Log::User(message);
    Log::Warning(message);
    Log::Error(message);

These rules refer to the classes and methods in the Python layer using
the services of the internal model and the solvers.

Python
------

Modules and packages
~~~~~~~~~~~~~~~~~~~~

*Example of tolerated notations:*

::

    import matplotlib
    from matplotlib import pylab
    import numpy as np

*Incorrect examples:*

::

    from scipy import *

Names
~~~~~

*Examples: RandomVector, Sample.*

*Examples:*

::

    rv = RandomVector()
    dim = rv.getDimension()

Comments and internal documentation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*Example of documentation string for the class
**AnotherSample**:*

::

    #
    # <detailed description for documentation tools such as HappyDoc>
    #
    class AnotherSample :
    """
    this class is designed to ...
    """
    #
    # <detailed description for developers and documentation tools>
    def __init__(self, name, type, range = None, doc = "") :
    """constructor -- """
    ...
    #
    # <detailed description for developers and documentation tools>
    def computeSomething(self, value):
    """run the well-known Schmoll Algorithm...
    """

Indentation
~~~~~~~~~~~

The python code should use 4 spaces per indentation level.
For more information about python formatting,
refer to `PEP8 <http://www.python.org/dev/peps/pep-0008/>`_.

