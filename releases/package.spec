Name:		frproxy
Version:	1.1.4
Release:	12
Summary:	Proxy of Service Framework Registry of Ganji RPC.

Group:		Ganji
License:	GNU
URL:		http://www.ganji.com
Source0:	frproxy-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
#BuildRequires:	 
#Requires:	

%define    userpath /usr/local/webserver/frproxy

%description
Proxy of Service Framework Registry of Ganji RPC.

%prep
#% setup -q %{buildroot}
echo ============================================================================ setup start1=
%setup -c

    test -d %{buildroot} || mkdir -p -- %{buildroot}
    ls %{buildroot}

    #cp -a frproxy-%{version}/* %{buildroot}

echo ============================================================================ setup end 1=

#%  build
#%  configure
#make %{?_smp_mflags}


%install
#    echo "installllllllllllllllllllllllllllllllllllll start"
#    pwd

    test -d %{buildroot}%{userpath} || mkdir -p -- %{buildroot}%{userpath}
    ls %{buildroot}
    cp -a frproxy-%{version}/* %{buildroot}%{userpath}

    ls %{buildroot}%{userpath}

#   echo "installllllllllllllllllllllllllllllllllllll end"

%clean
    rm -rf %{buildroot}
    rm -rf $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root,-)
%doc
%{userpath}/frproxy
%{userpath}/client
%{userpath}/libevent.a
%{userpath}/libevent.la
%{userpath}/libevent.so
%{userpath}/libthriftnb.so
%{userpath}/libthriftnb.so.0
%{userpath}/libthriftnb.so.0.0.0
%{userpath}/libthrift.so
%{userpath}/libthrift.so.0
%{userpath}/libthrift.so.0.0.0
%{userpath}/libzookeeper_mt.a
%{userpath}/libzookeeper_mt.la
%{userpath}/libzookeeper_mt.so
%{userpath}/libzookeeper_mt.so.2
%{userpath}/libzookeeper_mt.so.2.0.0
%{userpath}/proxy.sh
%{userpath}/proxy_client.sh
%{userpath}/README.md
%{userpath}/frproxy.init.d.sh

%changelog
