OpenTURNS (Open source initiative to Treat Uncertainties, Risks'N Statistics)

Tagline: OpenTURNS is a scientific C++ and Python library including an internal data model and algorithms
         dedicated to the treatment of uncertainties. The main function of that library is giving
         to specific applications all the functionalities needed to treat uncertainties in studies.
         Targeted users are all engineers who want to introduce the probabilistic dimension
         in their so far deterministic studies. 

Up-to-date OpenTURNS related information can be found at

        http://www.openturns.org

Other hints can be found on the OpenTURNS Wiki:

        http://trac.openturns.org/wiki

License
=======
OpenTURNS is distributed under the Lesser General Public License.
Please see the COPYING* files for details of the license of each components.

Release Notes
=============
Please see the NEWS file for a summary of bug fixes and
new features for the current release.

Backwards Compatibility
=======================
The developers strive their best to preserve backwards compatibility
between releases, but this is not always possible. Where backwards
compatibility is known to be broken, it is clearly marked as an
incompatibility in the NEWS file.

Binary Installation
====================
Please see the http://trac.openturns.org/wiki/HowToWindowsInstall
for instructions on installing OpenTURNS on Windows.
OpenTURNS is also available for various Linux platforms,
see http://trac.openturns.org/wiki/HowToLinuxInstall.

Source Installation
===================

Before trying to compile and install the software, be sure to get access to some
mandatory tools OpenTURNS relies upon. These are (at least):
 * CMake
 * LAPACK
 * SWIG
 * Python

For the quick readers:
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=~/.local ..
make install
make check
make installcheck

Don't skip the check stages. They ensure you the software you've just compiled behaves correctly.

Source Installation on Windows
==============================

Instructions above apply if you build in a Unix-like environment (Mingw, Cygwin).
If you want to built OpenTURNS using native tools, you first have to install:
 * CMake
 * Boost
 * muParser
 * libxml2
 * OpenBLAS (or MKL, or any LAPACK implementation)

Launch cmake GUI; builds have been tested with Visual Studio and NMake based generators.
Please note that Python bindings cannot be generated yet.

Documentation
=============
The http://trac.openturns.org/wiki/Documentation pages contains
the most recent set of updated documentation for this release. 

Participate!
============
Please report any errors and submit patches (if possible),
preferably on the user mailing list, see http://openturns.org/mailman/listinfo.
All contributions help.

 -- The OpenTURNS team
