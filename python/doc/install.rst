.. _install:

============
Installation
============

From binary packages
====================

Windows
-------
Download the installer matching your python version and architecture from `SourceForge <http://sourceforge.net/projects/openturns/files/openturns/>`_


Debian/Ubuntu
-------------

We support the following Debian-based distributions:

.. table::

    +-------------------------------------+----------------------------------------------+
    | Distribution name                   | Repository address                           |
    +=====================================+==============================================+
    | Ubuntu 12.04 LTS 'Precise Pangolin' | deb http://ubuntu.openturns.org precise main |
    +-------------------------------------+----------------------------------------------+
    | Ubuntu 14.04 LTS 'Trusty Tahr'      | deb http://ubuntu.openturns.org trusty main  |
    +-------------------------------------+----------------------------------------------+
    | Ubuntu 15.10 'Wily Werewolf'        | deb http://ubuntu.openturns.org wily main    |
    +-------------------------------------+----------------------------------------------+
    | Debian 7.0 'Wheezy'                 | deb http://debian.openturns.org wheezy main  |
    +-------------------------------------+----------------------------------------------+
    | Debian 8.0 'Jessie'                 | deb http://debian.openturns.org jessie main  |
    +-------------------------------------+----------------------------------------------+

Add OpenTURNS' mirror signature::

    curl http://debian.openturns.org/openturns.org-repo.key | sudo apt-key add -

To be able to retrieve OpenTURNS packages, you must add the appropriate
repository address to your sources list. To do so, you may either use the
graphical user interface of aptitude or you can edit the sources-list file
(`/etc/apt/sources.list`).

For instance, asuming you are running Ubuntu 14.04 'Trusty Tahr',
add the following source to your sources list::

    deb http://ubuntu.openturns.org trusty main

.. note::

    Use the bash command `lsb_release -c` in order to determine the codename of
    your distribution.

After editing aptitude's sources-list, you must update your packages database
using either the graphical interface or by issuing the following command::

    sudo aptitude update

The following packages should now be available:

.. table::

    +----------------------+------------------------------------+
    | Package name         | Description                        |
    +======================+====================================+
    | libopenturns0.x      | library                            |
    +----------------------+------------------------------------+
    | libopenturns-dev     | development package                |
    +----------------------+------------------------------------+
    | libopenturns0.x-dbg  | debug package                      |
    +----------------------+------------------------------------+
    | python-openturns     | Python module                      |
    +----------------------+------------------------------------+
    | python3-openturns    | Python 3 module                    |
    +----------------------+------------------------------------+
    | openturns-examples   | examples                           |
    +----------------------+------------------------------------+
    | openturns-validation | validation files                   |
    +----------------------+------------------------------------+
    | r-other-rot          | R module                           |
    +----------------------+------------------------------------+

Use the following command to check::

    apt-cache search openturns

RPM-based distributions
-----------------------
Add the repository corresponding to your operating system::

    curl http://download.opensuse.org/repositories/science:/openturns/CentOS_7/science:openturns.repo -o /etc/yum.repos.d/obs-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/Fedora_23/science:openturns.repo -o /etc/yum.repos.d/obs-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/openSUSE_Leap_42.1/science:openturns.repo -o /etc/yum.repos.d/obs-openturns.repo

Import the gpg key corresponding to your operating system::

    rpm --import http://download.opensuse.org/repositories/science:/openturns/CentOS_7/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/Fedora_23/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/openSUSE_Leap_42.1/repodata/repomd.xml.key

Install the package::

    yum install python-openturns

Conda
-----
This is relevant to the `Conda <http://conda.pydata.org/>`_ userland Python package manager.

Install the package::

    conda install -c https://conda.anaconda.org/openturns openturns


Archlinux/Parabola
------------------
Install from `AUR <https://aur.archlinux.org/packages/openturns/>`_ repository::

    yaourt -S openturns


From sources
============

Checkout the following dependencies:

- `CMake <http://www.cmake.org/>`_
- `LAPACK <http://www.netlib.org/lapack/>`_
- `LibXml2 <http://xmlsoft.org/>`_ (optional, for serialization)
- `muParser <http://muparser.beltoforion.de/>`_ (optional, for analytical parser)
- `SWIG <http://www.swig.org/>`_ (optional, for python bindings)
- `NLopt <http://ab-initio.mit.edu/nlopt>`_ (optional, for additional optimization algorithms)
- `Boost <http://www.boost.org/>`_ (optional, for efficient numerical functions)
- `TBB <http://www.threadingbuildingblocks.org/>`_ (optional, for multithreading)
- `Python <http://www.python.org/>`_ (optional, for python bindings)
- `Matplotlib <http://matplotlib.org/>`_ (optional, for viewer module)
- `Sphinx <http://sphinx-doc.org/>`_ (optional, for documentation)

You can checkout the development trunk::

    git clone https://github.com/openturns/openturns.git
    cd openturns
    cmake -DCMAKE_INSTALL_PREFIX=~/.local .
    make install

.. note::

    When installing into `~/.local` you will not need to set PYTHONPATH
    environment variable for Python to be able to import openturns
