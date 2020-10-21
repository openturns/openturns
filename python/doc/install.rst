.. _install:

============
Installation
============

From binary packages
====================

.. note::

    Since v1.15, only Python 3.x binaries are provided.

Pip
---
Install the package from `PyPI <https://pypi.org/project/openturns/>`_::

    pip3 install openturns

For an installation without administrative priviledges the option :code:`--user` can be used.
Note that pip does not install pre-releases unless given the option :code:`--pre`.

Conda
-----
This is relevant to the `Conda <http://conda.pydata.org/>`_ userland Python package manager.

On Linux/Mac, to avoid package conflicts we suggest installing conda from
`Miniconda <https://docs.conda.io/en/latest/miniconda.html>`_ instead of Anaconda::

    wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -P /tmp
    bash /tmp/Miniconda3-latest-Linux-x86_64.sh -b -p $HOME/miniconda
    PATH="$HOME/miniconda/bin:$PATH"

Then install from the `conda-forge <https://conda-forge.org>`_ channel::

    conda config --add channels conda-forge
    conda config --set channel_priority strict
    conda update -y conda
    conda install openturns

Alternatively, you can download the `otconda <https://github.com/openturns/otconda>`_ bundle
containing the library and its modules that allows for an offline installation.
Note that pre-releases are published on conda-forge like regular releases, so
if you want to stick to the stable version you will have to explicitely specify
the required version, eg :code:`conda install openturns=1.16`.

Windows
-------
Download the installer matching your python version and architecture from `GitHub <https://github.com/openturns/build/releases>`_
Then just run installer and follow the instructions.

A command-line installation is also possible::

    openturns-1.16-py38-x86_64.exe /userlevel=1 /S /FORCE /D=%PREFIX%

Debian/Ubuntu
-------------

We support the following Debian-based distributions:

.. table::

    +-------------------------------------+---------------------------------------------------------+
    | Distribution name                   | Repository address                                      |
    +=====================================+=========================================================+
    | Ubuntu 16.04 LTS 'Xenial Xerus'     | deb https://openturns.github.io/apt/ubuntu xenial main  |
    +-------------------------------------+---------------------------------------------------------+
    | Ubuntu 18.04 LTS 'Bionic Beaver'    | deb https://openturns.github.io/apt/ubuntu bionic main  |
    +-------------------------------------+---------------------------------------------------------+
    | Ubuntu 20.04 LTS 'Focal Fossa'      | deb https://openturns.github.io/apt/ubuntu focal main   |
    +-------------------------------------+---------------------------------------------------------+
    | Debian 9.0 'Stretch'                | deb https://openturns.github.io/apt/debian stretch main |
    +-------------------------------------+---------------------------------------------------------+
    | Debian 10.0 'Buster'                | deb https://openturns.github.io/apt/debian buster main  |
    +-------------------------------------+---------------------------------------------------------+

Add the mirror signature::

    curl openturns.github.io/apt/public.key | sudo apt-key add -

To be able to retrieve packages, you must add the appropriate
repository address to your sources list. To do so, you may either use the
graphical user interface of aptitude or you can edit the sources-list file
(`/etc/apt/sources.list`).

For instance, assuming you are running Ubuntu 20.04,
add the following source to your sources list::

    echo deb https://openturns.github.io/apt/ubuntu focal main | sudo tee /etc/apt/sources.list.d/openturns.list

.. note::

    Use the bash command `lsb_release -c` in order to determine the codename of
    your distribution.

You may want to enable apt https support to be able to fetch from github.io::

    sudo apt install apt-transport-https

After editing aptitude's sources-list, you must update your packages database
using either the graphical interface or by issuing the following command::

    sudo apt update

The following packages are available:

.. table::

    +----------------------+------------------------------------+
    | Package name         | Description                        |
    +======================+====================================+
    | libopenturns0.x      | library                            |
    +----------------------+------------------------------------+
    | libopenturns-dev     | development package                |
    +----------------------+------------------------------------+
    | python3-openturns    | Python module                      |
    +----------------------+------------------------------------+

To install the Python module::

    sudo apt install python3-openturns

RPM-based distributions
-----------------------
Add the repository corresponding to your operating system::

    curl http://download.opensuse.org/repositories/science:/openturns/CentOS_8/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/Fedora_32/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/openSUSE_Leap_15.2/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo
    curl http://download.opensuse.org/repositories/science:/openturns/Mageia_7/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo

Import the gpg key corresponding to your operating system::

    rpm --import http://download.opensuse.org/repositories/science:/openturns/CentOS_8/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/Fedora_32/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/openSUSE_Leap_15.2/repodata/repomd.xml.key
    rpm --import http://download.opensuse.org/repositories/science:/openturns/Mageia_7/repodata/repomd.xml.key

The following packages are available:

.. table::

    +----------------------+------------------------------------+
    | Package name         | Description                        |
    +======================+====================================+
    | openturns-libs       | library                            |
    +----------------------+------------------------------------+
    | openturns-devel      | development package                |
    +----------------------+------------------------------------+
    | python3-openturns    | Python module                      |
    +----------------------+------------------------------------+

To install the Python module::

    yum install python3-openturns

Archlinux
---------
Install from `AUR <https://aur.archlinux.org/packages/openturns/>`_::

    aurman -S openturns

FreeBSD
-------
Install from `FreshPorts <https://www.freshports.org/math/openturns/>`_::

    pkg install openturns


From sources
============

Checkout the required :ref:`dependencies`.

You can checkout the development trunk::

    git clone https://github.com/openturns/openturns.git
    cd openturns
    cmake -DCMAKE_INSTALL_PREFIX=~/.local .
    make install

.. note::

    When installing into `~/.local` you dont need to set PYTHONPATH
    environment variable for Python to be able to import openturns
