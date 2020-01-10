# make sure your Linux distribution has support for multilib for 32-bit execs:
#
# https://askubuntu.com/questions/470796/fatal-error-sys-cdefs-h-no-such-file-or-directory
#
# gcc hbg*.c -o hbg
# gcc -DUSE_DICE -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -o engine engine.c
gcc -m32 -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -c engine0.c
gcc -m32 -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -c engine1.c
gcc -m32 -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -c engine2.c
gcc -m32 -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -c engine3.c
gcc -m32 -DDEBUG -Wall -Wstrict-prototypes -ansi -pedantic -o engine engine.c engine0.o engine1.o engine2.o engine3.o

# https://stackoverflow.com/questions/38786014/how-to-compile-executable-for-windows-with-gcc-with-linux-subsystem
# sudo apt-get install mingw-w64
# Suggested packages:
#  gcc-5-locales wine wine64

# i686-w64-mingw32-gcc -o main32.exe main.c

# x86_64-w64-mingw32-gcc -o main64.exe main.c

i686-w64-mingw32-gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine0.c 
i686-w64-mingw32-gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine1.c 
i686-w64-mingw32-gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine2.c 
i686-w64-mingw32-gcc -Wall -Wstrict-prototypes -ansi -pedantic -c engine3.c 
i686-w64-mingw32-gcc -Wall -Wstrict-prototypes -ansi -pedantic -o engine32.exe engine.c engine0.o engine1.o engine2.o engine3.o

# http://www.pcg-random.org/

# Suggested packages:
#  aspell-doc spellutils cups update-inetd xpp smbclient wordlist ghostscript-x hpijs hunspell openoffice.org-hunspell | openoffice.org-core
#  libterm-readline-gnu-perl | libterm-readline-perl-perl imagemagick-doc autotrace curl enscript ffmpeg gimp gnuplot grads graphviz hp2xx html2ps
#  libwmf-bin mplayer povray radiance sane-utils texlive-base-bin transfig ufraw-batch isdnutils-doc isdnutils-doc:i386 libdv-bin oss-compat
#  libenchant-voikko rng-tools:i386 libgd-tools:i386 gnutls-bin:i386 gphoto2:i386 gpm gpm:i386 fonts-droid texlive-lang-cjk krb5-doc:i386
#  krb5-user:i386 jackd2:i386 liblcms2-utils:i386 inkscape libmyodbc odbc-postgresql tdsodbc unixodbc-bin libportaudio2 libportaudio2:i386
#  pciutils:i386 libraw1394-doc avahi-daemon:i386 hplip:i386 libsane-extras:i386 sane-utils:i386 libsasl2-modules-otp:i386 libsasl2-modules-ldap:i386
#  libsasl2-modules-sql:i386 libsasl2-modules-gssapi-mit:i386 | libsasl2-modules-gssapi-heimdal:i386 lm-sensors:i386 libwmf0.2-7-gtk opencl-icd
#  opencl-icd:i386 poppler-utils fonts-japanese-mincho | fonts-ipafont-mincho fonts-japanese-gothic | fonts-ipafont-gothic fonts-arphic-ukai
#  fonts-arphic-uming fonts-nanum python-dbus-doc python3-dbus-dbg dosbox:any winbind
# Recommended packages:
#  xml-core:i386 fonts-droid
