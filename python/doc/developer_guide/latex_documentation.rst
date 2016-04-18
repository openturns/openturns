.. _latex_doc:

Latex documentation
===================

This section deals with the old Latex documentation, located in a separate repository
and having it's own releases.

The documentation is mainly 3-folds. It contains:

  - Use Cases Guide
  - Examples Guide
  - Reference Guide, which is more literate and deals with the
    theoretical background of the methods featured in the library.


Retrieve the documentation
--------------------------

#. Install the required tools: tralics, xsltproc and the usual latex
   packages.

#. Fork the repository https://github.com/openturns/doc.git:

   ::

        git clone git@github.com:YOURUSERNAME/doc.git


Develop the documentation
-------------------------

#. Add your contribution, update the CMakeLists when adding new files.

#. Build the documentation:

   ::

        mkdir build
        cd build
        cmake -DCMAKE_INSTALL_PREFIX=$PWD/install ..
        make

#. Check the PDF and HTML outputs

   ::

        make install
        xpdf install/share/doc/openturns-doc/pdf/OpenTURNS_ReferenceGuide.pdf
        firefox install/share/doc/openturns-doc/html/ReferenceGuide/index.xhtml


Run embedded scripts
--------------------

#. Detect your openturns installation:

   ::

        cmake -DUSE_OPENTURNS=ON \
          -DOpenTURNS_DIR=<OT_PREFIX>/lib/cmake/openturns ..


#. Run tests

   ::

        make check

   and all the tests should be successful else check the log file
   test/Testing/Temporary/LastTest.log.
