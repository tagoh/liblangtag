#! /bin/sh

srcdir=$1
shift

while [ $# -ne 0 ]; do
    f=$1
    shift
    x=`echo $f|sed -e 's,\.\([ch]\),.gir.\1,g'`
    echo "$x: $srcdir/$f \$(srcdir)/gengir.sh Makefile"
    echo "	\$(AM_V_GEN) \$(srcdir)/gengir.sh \$(top_srcdir)/liblangtag \$@"
done
echo "\$(srcdir)/gendir.sh:"
