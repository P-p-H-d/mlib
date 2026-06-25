# Packaging of header-only C library
# https://docs.fedoraproject.org/en-US/packaging-guidelines/#_packaging_header_only_libraries
%global debug_package %{nil}

Name: m-lib
Version: 0.8.0
Release: 1%{?dist}
Summary: Type-safe containers for C language
URL: https://github.com/P-p-H-d/mlib
License: BSD-2-Clause
Source0: %{url}/archive/V%{version}/%{name}-%{version}.tar.gz

BuildRequires: make
BuildRequires: gcc
BuildRequires: coreutils
BuildRequires: pkgconfig

%description
M*LIB is a library of generic and type safe containers / data structures
in pure C language (C99 / C11) for a wide collection of container
(comparable to the C++ STL). 

%package devel
Summary: Development files for M*LIB library
BuildArch: noarch
Provides: %{name}-static = %{version}-%{release}
%description devel
Development files needed to build and link to the M*LIB library.


%prep
%setup -q -n MLIB-%{version}


%build
%make_build


%install
%make_install PREFIX=%{_prefix} INSTALL_DOCS=no


%check
%make_build check


%files
%license LICENSE

%files devel
%license LICENSE
%doc README.md
%{_includedir}/m-lib/
%{_datadir}/pkgconfig/m-lib.pc


%changelog
* Tue Jun 23 2026 Brian Sipos <brian.sipos@gmail.com> - 0.8.0-1
- Import into Fedora
