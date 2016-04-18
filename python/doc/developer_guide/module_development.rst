Module development
==================

This section deals with the process of creation of a new extra module.

Adapt an existing template
--------------------------

#. Fork the C++ template module repository 
   `OTTemplate <https://github.com/openturns/ottemplate>`_,
   rename it from the project settings and clone it:

   ::

        git clone https://github.com/yourusername/otmymodule.git


#. Adapt the template to your module:

   ::

        ./customize.sh OTMyModule MySuperClass

   This command changes the module name into MyModule in all the
   scripts, and adapt the example class to the new name MyClass.

   The script can then be discarded:

   ::

        git rm customize.sh
        git commit customize.sh -m "Removed customize script"
        
#. Set the version of your module:

   ::

        ./setVersionNumber.sh 1.0


   This command changes the module version, which is 0.0 by default.

Develop the module
------------------

#. Implement your module using the same rules as described in the
   library development section.

#. Build your module as usual:

   ::

        mkdir build && cd build
        cmake -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DOpenTURNS_DIR=OT_PREFIX/lib/cmake/openturns ..
        make
        make check
        make install
        make installcheck


Install and test
----------------

#. Check that you have a working OpenTURNS installation, for example by
   trying to load the OpenTURNS module:

   ::

           python -c "import openturns as ot; print(ot.__version__)"
         

   and python should not complain about a non existing openturns module.

#. Test your module within python: Adjust your PYTHONPATH if necessary

   ::

           python
           >>> import mymodule
         

   and python should not complain about a non existing mymodule module.

#. Create a source package of your module:

   ::

           make package_source
         

   It will create a tarball named mymodule-X.Y.tar.gz (and
   mymodule-X.Y.tar.bz2), where X.Y is the version number of the module.

