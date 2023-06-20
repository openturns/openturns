# norootforbuild
%{?__python3: %global __python %{__python3}}
%if 0%{?suse_version}
%global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")
%else
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}
%endif

%define __cmake %{_bindir}/cmake
%define cmake \
CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ; \
CXXFLAGS="${CXXFLAGS:-%optflags} -fno-lto" ; export CXXFLAGS ; \
FFLAGS="${FFLAGS:-%optflags}" ; export FFLAGS ; \
%__cmake \\\
-DCMAKE_INSTALL_PREFIX:PATH=%{_prefix} \\\
-DBUILD_SHARED_LIBS:BOOL=ON

Name:           openturns
Version:        1.21
Release:        1%{?dist}
Summary:        Uncertainty treatment library
Group:          System Environment/Libraries
License:        LGPLv3+
URL:            http://www.openturns.org
Source0:        http://downloads.sourceforge.net/openturns/openturns/openturns-%{version}.tar.bz2
Source1:        %{name}-rpmlintrc
BuildRequires:  gcc-c++, cmake, bison, flex, swig
BuildRequires:  libxml2-devel
%if ! 0%{?centos_version}
BuildRequires:  hdf5-devel
%endif
BuildRequires:  boost-devel
%if 0%{?suse_version}
BuildRequires:  mpc-devel
%else
BuildRequires:  libmpc-devel
%endif
BuildRequires:  nlopt-devel
BuildRequires:  tbb-devel
%if 0%{?fedora_version} || 0%{?mageia}
BuildRequires:  pagmo-devel
%endif
BuildRequires:  python3-devel
%if !0%{?mageia}
BuildRequires:  hmat-oss-devel
%endif
BuildRequires:  spectra-devel
BuildRequires:  cminpack-devel
%if 0%{?fedora_version}
BuildRequires:  ceres-solver-devel
BuildRequires:  suitesparse-devel
BuildRequires:  coin-or-Ipopt-devel
BuildRequires:  coin-or-Bonmin-devel
BuildRequires:  dlib-devel, pkgconfig(x11), pkgconfig(libpng), pkgconfig(libjpeg), pkgconfig(sqlite3)
BuildRequires:  flexiblas-devel
BuildRequires:  primesieve-devel
%else
BuildRequires:  lapack-devel
%endif

%description
OpenTURNS Uncertainty treatment library

%package libs
Summary:        Uncertainty treatment library
Group:          Development/Libraries/C and C++

%description libs
Uncertainty treatment library binaries 

%package devel
Summary:        OpenTURNS development files
Group:          Development/Libraries/C and C++
Requires:       %{name}-libs = %{version}
%if !0%{?mageia}
Requires:       hmat-oss-devel
%endif

%description devel
Development files for OpenTURNS uncertainty library

%package -n python3-%{name}
Summary:        Uncertainty treatment library
Group:          Productivity/Scientific/Math
Requires:       python3
Requires:       %{name}-libs = %{version}
%if ! 0%{?centos_version}
Requires:       python3-dill
Requires:       python3-psutil
%endif

%description -n python3-%{name}
Python textual interface to OpenTURNS uncertainty library

%prep
%setup -q

%build
%cmake -DINSTALL_DESTDIR:PATH=%{buildroot} \
       -DCMAKE_SKIP_INSTALL_RPATH:BOOL=ON \
       -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
       -DSWIG_COMPILE_FLAGS="-O1" \
       -DOPENTURNS_SYSCONFIG_PATH=/etc .
make %{?_smp_mflags} OT
make

%install
make install DESTDIR=%{buildroot}
rm -r %{buildroot}%{_datadir}/%{name}/doc

%check
LD_LIBRARY_PATH=%{buildroot}%{_libdir} OPENTURNS_NUM_THREADS=1 ctest --output-on-failure %{?_smp_mflags} -E "cppcheck|ChaosSobol|Kriging" --timeout 1000 --schedule-random || echo "fail"

%post libs -p /sbin/ldconfig
%postun libs -p /sbin/ldconfig

%files libs
%defattr(-,root,root,-)
%doc COPYING README.rst
%dir %{_sysconfdir}/%{name}
%config %{_sysconfdir}/%{name}/%{name}.conf
%{_libdir}/*.so.*
%dir %{_datadir}/%{name}
%{_datadir}/gdb/

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h*
%{_includedir}/%{name}/swig/
%{_libdir}/*.so
%{_libdir}/cmake/

%files -n python3-%{name}
%defattr(-,root,root,-)
%{python_sitearch}/%{name}/
%{python_sitearch}/%{name}-*.dist-info/

%changelog
* Tue May 16 2023 Julien Schueller <schueller at phimeca dot com> 1.21-1
- New upstream release

* Mon Nov 02 2022 Julien Schueller <schueller at phimeca dot com> 1.20-1
- New upstream release

* Tue Apr 12 2022 Julien Schueller <schueller at phimeca dot com> 1.19-1
- New upstream release

* Fri Oct 15 2021 Julien Schueller <schueller at phimeca dot com> 1.18-1
- New upstream release

* Mon Apr 19 2021 Julien Schueller <schueller at phimeca dot com> 1.17-1
- New upstream release

* Mon Oct 19 2020 Julien Schueller <schueller at phimeca dot com> 1.16-1
- New upstream release

* Mon Apr 6 2020 Julien Schueller <schueller at phimeca dot com> 1.15-1
- New upstream release

* Tue Nov 12 2019 Julien Schueller <schueller at phimeca dot com> 1.14-1
- New upstream release

* Tue Apr 23 2019 Julien Schueller <schueller at phimeca dot com> 1.13-1
- New upstream release

* Tue Oct 2 2018 Julien Schueller <schueller at phimeca dot com> 1.12-1
- New upstream release

* Mon Apr 9 2018 Julien Schueller <schueller at phimeca dot com> 1.11-1
- New upstream release

* Wed Oct 11 2017 Julien Schueller <schueller at phimeca dot com> 1.10-1
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
