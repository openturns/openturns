# norootforbuild
%define r_library %{_libdir}/R/library
%define packname rot

Name:             R-%{packname}
Version:          1.4.6
Release:          1%{?dist}
Source:           %{packname}_%{version}.tar.gz
License:          LGPLv2
URL:              http://www.openturns.org
Group:            Applications/Engineering
Summary:          R package for OpenTURNS
%if 0%{?suse_version}
BuildRequires:    R-base-devel >= 2.0
%endif
%if 0%{?mdkversion}
BuildRequires:    R-base >= 2.0
%endif
%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version} || 0%{?scientificlinux_version}
BuildRequires:    R-devel >= 2.0
%endif
%if 0%{?rhel_version} || 0%{?centos_version} || 0%{?mdkversion}   
BuildRequires:    tetex-latex
%endif  
%if 0%{?fedora} >= 15
BuildRequires:    sane-backends-libs
%endif
%if 0%{?suse_version}
BuildRequires:    texlive-latex 
%endif
%if 0%{?suse_version} || 0%{?mdkversion}
Requires(post):   R-base >= 2.0
Requires(postun): R-base >= 2.0
%endif

%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version} 
Requires(post):   R-core >= 2.0
Requires(postun): R-core >= 2.0
%endif
BuildRoot:        %{_tmppath}/%{name}-%{version}-%{release}-root

%description
R package for OpenTURNS enabling additional statistical capabilities

%prep
%setup -q -c -n %{packname}

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{r_library}
%{_bindir}/R CMD INSTALL -l %{buildroot}%{r_library} %{packname}
test -d %{packname}/src && (cd %{packname}/src; rm -f *.o *.so)
rm -f %{buildroot}%{r_library}/R.css

%check
%if 0%{?fedora} >= 15
%else
%{_bindir}/R CMD check %{packname}
%endif

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%dir %{r_library}/%{packname}
%doc %{r_library}/%{packname}/html
%doc %{r_library}/%{packname}/DESCRIPTION
%{r_library}/%{packname}/NAMESPACE
%{r_library}/%{packname}/INDEX
%{r_library}/%{packname}/Meta
%{r_library}/%{packname}/R
%{r_library}/%{packname}/help

%changelog
* Sat Nov 30 2012 Julien Schueller <schueller at phimeca dot com> 1.4.5-1
- New upstream release

* Sat Oct 9 2010 Julien Schueller <schueller at phimeca dot com> 1.4.4-1
- New upstream release

* Mon Nov 26 2007 Remy Pagniez
- Initial package creation
