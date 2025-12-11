# norootforbuild
%global python_sitearch %{_libdir}/python%(python3 -c "import sysconfig; print(sysconfig.get_python_version())")/site-packages

Name:           openturns
Version:        1.26
Release:        1%{?dist}
Summary:        Uncertainty treatment library
Group:          System Environment/Libraries
License:        LGPLv3+
URL:            http://www.openturns.org
Source0:        http://downloads.sourceforge.net/openturns/openturns/openturns-%{version}.tar.bz2
Source1:        %{name}-rpmlintrc
BuildRequires:  gcc-c++, cmake, swig
BuildRequires:  libxml2-devel
BuildRequires:  hdf5-devel
BuildRequires:  nlopt-devel
BuildRequires:  tbb-devel
BuildRequires:  cuba-devel
BuildRequires:  pagmo2-devel
BuildRequires:  python3-devel
BuildRequires:  spectra-devel
BuildRequires:  cminpack-devel
BuildRequires:  nanoflann-devel
%if 0%{?fedora_version}
BuildRequires:  coin-or-Ipopt-devel
BuildRequires:  coin-or-Bonmin-devel
BuildRequires:  zlib-devel
BuildRequires:  flexiblas-devel
%else
BuildRequires:  lapack-devel
BuildRequires:  highs-devel
%endif
%if 0%{?suse_version}
BuildRequires:  mpc-devel
BuildRequires:  cblas-devel
BuildRequires:  Ipopt-devel
BuildRequires:  libprimesieve-devel
%else
BuildRequires:  libmpc-devel
BuildRequires:  boost-devel
BuildRequires:  primesieve-devel
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

%description devel
Development files for OpenTURNS uncertainty library

%package -n python3-%{name}
Summary:        Uncertainty treatment library
Group:          Productivity/Scientific/Math
Requires:       python3
Requires:       %{name}-libs = %{version}
Requires:       python3-dill
Requires:       python3-psutil
Requires:       python3-packaging

%description -n python3-%{name}
Python textual interface to OpenTURNS uncertainty library

%prep
%setup -q

%build
%cmake -DINSTALL_DESTDIR:PATH=%{buildroot} \
       -DCMAKE_SKIP_INSTALL_RPATH:BOOL=ON \
       -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
       -DSWIG_COMPILE_FLAGS="-O1" \
       -DOPENTURNS_DOC_PATH=share/doc/openturns \
       -DOPENTURNS_SYSCONFIG_PATH=/etc/openturns .
%cmake_build

%install
%cmake_install
rm -r %{buildroot}%{_datadir}/doc/%{name}

%check
export LD_LIBRARY_PATH=%{buildroot}%{_libdir}
export OMP_NUM_THREADS=1
export OPENTURNS_NUM_THREADS=1
%ctest --tests-regex pyinstallcheck --schedule-random --timeout 1000

%post libs -p /sbin/ldconfig
%postun libs -p /sbin/ldconfig

%files libs
%defattr(-,root,root,-)
%doc COPYING README.rst
%dir %{_sysconfdir}/%{name}
%config %{_sysconfdir}/%{name}/%{name}.conf
%{_libdir}/*.so.*
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
* Wed Nov 19 2025 Julien Schueller <schueller at phimeca dot com> 1.26-1
- New upstream release
