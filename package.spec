Name:		frproxy
Version:	1.0.0
Release:	1
Summary:	Proxy of Service Framework Registry of Ganji RPC.

Group:		Ganji
License:	GNU
URL:		http://www.ganji.com
Source0:	frproxy-1.0.0.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
#BuildRequires:	 
#Requires:	

%define    userpath /usr/local/webserver/frproxy

%description
Proxy of Service Framework Registry of Ganji RPC.

%prep
#% setup -q %{buildroot}
#echo ============================================================================ setup start1=
%setup -c

test -d %{buildroot} || mkdir -- %{buildroot}
cp -a frproxy-1.0.0/* %{buildroot}
#echo ============================================================================ setup end 1=

#%  build
#%  configure
#make %{?_smp_mflags}


%install
#rm -rf %{buildroot}
    #make install DESTDIR=%{buildroot}
#    echo ============================================================================installation start=
    PWD_BACKUP=$PWD
    cd %{buildroot}

    install -m 777 frproxy /usr/local/webserver/frproxy/
    install -m 777 proxy.sh /usr/local/webserver/frproxy/
    install -m 666 -t /usr/local/webserver/frproxy/ lib*
    install -m 666 -t /usr/local/webserver/frproxy/ README.md
    echo '!@#$%^&*()###########################################################################'
    cd $PWD_BACKUP 
#   echo ============================================================================installation start=
#install -d $RPM_BUILD_ROOT%{userpath}
#cp -a %{name}* $RPM_BUILD_ROOT%{userpath}


%clean
rm -rf %{buildroot}
#rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(-,root,root,-)
%doc
/frproxy
/libevent.a
/libevent.la
/libevent.so
/libthriftnb.so
/libthriftnb.so.0
/libthriftnb.so.0.0.0
/libthrift.so
/libthrift.so.0
/libthrift.so.0.0.0
/libzookeeper_mt.a
/libzookeeper_mt.la
/libzookeeper_mt.so
/libzookeeper_mt.so.2
/libzookeeper_mt.so.2.0.0
/proxy.sh
/README.md

%changelog
* Wed Jan 27 2010 Charles Channelman <channelman@stationX.example.com> - 1.0-1  
- Initial build. 
