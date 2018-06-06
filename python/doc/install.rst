.. _install:

============
Installation
============

From binary packages
====================

Pip
---
Install the package from `PyPI <https://pypi.org/project/openturns/>`_::

    pip install openturns

For an installation without administrative priviledges the option :code:`--user` can be used.

Conda
-----
This is relevant to the `Conda <http://conda.pydata.org/>`_ userland Python package manager.

Install the package from `Anaconda.org <https://anaconda.org/conda-forge/openturns>`_::

    conda config --add channels conda-forge
    conda install openturns

Alternatively, you can download the `otconda <https://github.com/openturns/otconda>`_ bundle
containing the library and its modules that allows for an offline installation.

Windows
-------
Download the installer matching your python version and architecture from `GitHub <https://github.com/openturns/build/releases>`_

Debian/Ubuntu
-------------

We support the following Debian-based distributions:

.. table::

    +-------------------------------------+----------------------------------------------+
    | Distribution name                   | Repository address                           |
    +=====================================+==============================================+
    | Ubuntu 14.04 LTS 'Trusty Tahr'      | deb http://ubuntu.openturns.org trusty main  |
    +-------------------------------------+----------------------------------------------+
    | Ubuntu 16.04 LTS 'Xenial Xerus'     | deb http://ubuntu.openturns.org xenial main  |
    +-------------------------------------+----------------------------------------------+
    | Ubuntu 18.04 LTS 'Bionic Beaver'    | deb http://ubuntu.openturns.org bionic main  |
    +-------------------------------------+----------------------------------------------+
    | Debian 8.0 'Jessie'                 | deb http://debian.openturns.org jessie main  |
    +-------------------------------------+----------------------------------------------+
    | Debian 9.0 'Stretch'                | deb http://debian.openturns.org stretch main |
    +-------------------------------------+----------------------------------------------+

Add the mirror signature::

    curl http://debian.openturns.org/openturns.org-repo.key | sudo apt-key add -

To be able to retrieve packages, you must add the appropriate
repository address to your sources list. To do so, you may either use the
graphical user interface of aptitude or you can edit the sources-list file
(`/etc/apt/sources.list`).

For instance, assuming you are running Ubuntu 16.04,
add the following source to your sources list::

    echo deb http://ubuntu.openturns.org xenial main | sudo tee /etc/apt/sources.list.d/openturns.list

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
    curl http://download.opensuse.org/repositories/science:/openturns/Fedora_28/science:openturns.repo -o /etc/yum.repos.d/obs-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/openSUSE_Leap_42.3/science:openturns.repo -o /etc/yum.repos.d/obs-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/Mageia_6/science:openturns.repo -o /etc/yum.repos.d/obs-openturns.repo

Import the gpg key corresponding to your operating system::

    rpm --import http://download.opensuse.org/repositories/science:/openturns/CentOS_7/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/Fedora_28/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/openSUSE_Leap_42.3/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/Mageia_6/repodata/repomd.xml.key

Install the package::

    yum install python-openturns

Archlinux
---------
Install from `AUR <https://aur.archlinux.org/packages/openturns/>`_::

    aurman -S openturns


From sources
============

Checkout the required :ref:`dependencies`.

You can checkout the development trunk::

    git clone https://github.com/openturns/openturns.git
    cd openturns
    cmake -DCMAKE_INSTALL_PREFIX=~/.local .
    make install

.. note::

    When installing into `~/.local` you will not need to set PYTHONPATH
    environment variable for Python to be able to import openturns
