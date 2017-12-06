# norootforbuild
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}

%define __cmake %{_bindir}/cmake
%define _cmake_lib_suffix64 -DLIB_SUFFIX=64
%define cmake \
CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ; \
CXXFLAGS="${CXXFLAGS:-%optflags}" ; export CXXFLAGS ; \
FFLAGS="${FFLAGS:-%optflags}" ; export FFLAGS ; \
%__cmake \\\
-DCMAKE_INSTALL_PREFIX:PATH=%{_prefix} \\\
%if "%{?_lib}" == "lib64" \
%{?_cmake_lib_suffix64} \\\
%endif \
-DBUILD_SHARED_LIBS:BOOL=ON

Name:           openturns
Version:        1.10
Release:        1%{?dist}
Summary:        Uncertainty treatment library
Group:          System Environment/Libraries
License:        LGPLv3+
URL:            http://www.openturns.org
Source0:        http://downloads.sourceforge.net/openturns/openturns/openturns-%{version}.tar.bz2
Source1:        %{name}-rpmlintrc
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:  gcc-c++, cmake, bison, flex, bc, swig
BuildRequires:  muParser-devel
BuildRequires:  libxml2-devel
BuildRequires:  boost-devel
BuildRequires:  nlopt-devel
#BuildRequires:  hmat-oss-devel
%if 0%{?suse_version}
BuildRequires:  lapack
%else
BuildRequires:  lapack-devel
%endif
%if 0%{?suse_version}
BuildRequires:  gcc-fortran
%else
BuildRequires:  gcc-gfortran
%endif
BuildRequires:  python-devel

%description
OpenTURNS Uncertainty treatment library

%package libs
Summary:        Uncertainty treatment library
Group:          Development/Libraries/C and C++
%if ! 0%{?mageia}
Requires:       muParser
Requires:       lapack
%else
Requires:       lib64muparser2
Requires:       liblapack
%endif
Requires:       libxml2
%if ! 0%{?suse_version}
Requires:       nlopt
%endif
#Requires:       libhmat-oss1

%description libs
Uncertainty treatment library binaries 

%package devel
Summary:        OpenTURNS development files
Group:          Development/Libraries/C and C++
Requires:       %{name}-libs = %{version}
Requires:       muParser-devel
Requires:       libxml2-devel
%if ! 0%{?suse_version}
Requires:       lapack-devel
%endif
Requires:       nlopt-devel

%description devel
Development files for OpenTURNS uncertainty library

%package examples
Summary:        OpenTURNS examples
Group:          Productivity/Scientific/Math

%description examples
OpenTURNS python examples

%package validation
BuildArch:      noarch
Summary:        OpenTURNS validation files
Group:          Productivity/Scientific/Math

%description validation
OpenTURNS validation text files

%package -n python-%{name}
Summary:        Uncertainty treatment library
Group:          Productivity/Scientific/Math
Requires:       python
Requires:       %{name}-libs = %{version}

%description -n python-%{name}
Python textual interface to OpenTURNS uncertainty library

%prep
%setup -q

%build
%cmake -DINSTALL_DESTDIR:PATH=%{buildroot} \
       -DCMAKE_SKIP_INSTALL_RPATH:BOOL=ON \
       -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES=-j8 \
%ifarch i586 i686
       -DCMAKE_C_FLAGS_RELEASE="%optflags -O0" \
%endif
       -DOPENTURNS_SYSCONFIG_PATH=/etc .
make %{?_smp_mflags} OT
make

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
rm -r %{buildroot}%{_datadir}/%{name}/doc

%check
make tests %{?_smp_mflags}
LD_LIBRARY_PATH=%{buildroot}%{_libdir} ctest %{?_smp_mflags} || cat Testing/Temporary/LastTest.log
rm %{buildroot}%{python_sitearch}/%{name}/*.pyc

%clean
rm -rf %{buildroot}

%post libs -p /sbin/ldconfig
%postun libs -p /sbin/ldconfig

%files libs
%defattr(-,root,root,-)
%doc COPYING README.rst
%dir %{_sysconfdir}/%{name}
%config %{_sysconfdir}/%{name}/%{name}.conf
%{_libdir}/*.so.*
%dir %{_datadir}/%{name}
%{_datadir}/gdb/auto-load%{_libdir}/libOT*-gdb.py*

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h*
%{_includedir}/%{name}/swig/
%{_libdir}/*.so
%{_libdir}/cmake/

%files examples
%defattr(-,root,root,-)
%{_datadir}/%{name}/examples/

%files validation
%defattr(-,root,root,-)
%{_datadir}/%{name}/validation/

%files -n python-%{name}
%defattr(-,root,root,-)
%{python_sitearch}/%{name}/

%changelog
* Mon Oct 11 2017 Julien Schueller <schueller at phimeca dot com> 1.10-1
- New upstream release

* Mon Apr 3 2017 Julien Schueller <schueller at phimeca dot com> 1.9-1
- New upstream release

* Thu Jun 30 2016 Julien Schueller <schueller at phimeca dot com> 1.8-1
- New upstream release

* Mon Dec 7 2015 Julien Schueller <schueller at phimeca dot com> 1.7-1
- New upstream release

* Mon Jun 15 2015 Julien Schueller <schueller at phimeca dot com> 1.6-1
- New upstream release

* Wed Dec 3 2014 Julien Schueller <schueller at phimeca dot com> 1.5-1
- New upstream release

* Wed Jul 2 2014 Julien Schueller <schueller at phimeca dot com> 1.4-1
- New upstream release

* Tue Dec 17 2013 Julien Schueller <schueller at phimeca dot com> 1.3-1
- New upstream release

* Mon Jul 22 2013 Julien Schueller <schueller at phimeca dot com> 1.2-1
- New upstream release

* Wed Nov 28 2012 Julien Schueller <schueller at phimeca dot com> 1.1-1
- New upstream release

* Sat Feb 18 2012 Julien Schueller <schueller at phimeca dot com> 1.0-1
- New upstream release

* Sat Jul 30 2011 Julien Schueller <schueller at phimeca dot com> 0.15-1
- New upstream release

* Sat Apr 9 2011 Julien Schueller <schueller at phimeca dot com> 0.14.0-1
- New upstream release

* Sat Oct 9 2010 Julien Schueller <schueller at phimeca dot com> 0.13.2-1
- New upstream release

* Mon Nov 26 2007 Remy Pagniez 0.11.1-1
- Initial package creation
