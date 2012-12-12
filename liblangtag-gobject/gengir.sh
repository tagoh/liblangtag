#! /bin/sh

type_capitalize() {
    __type=`echo $1|sed -e 's/_t$//'`
    __Type=''
    old_IFS=$IFS
    IFS='_'
    for i in $__type; do
	__Type="$__Type`echo $i|sed -e 's/./\U&/'`"
    done
    echo $__Type
}

guess_type() {
    echo "`echo $1|sed -e 's/-/_/g'`_t"
}

capitalize() {
    __in=$1
    __out=$2
    __cl=$3
    __Cl=`type_capitalize $__cl`
    __tmpsed=`mktemp gengir.XXXXXXXX`
    printf "s/^${__cl}/${__Cl}/g
s/\(const[ \t].*\)${__cl}/\\\1${__Cl}/g
s/\(typedef[ \t].*\)${__cl}/\\\1${__Cl}/g
s/\([ \t].*\)${__cl};/\\\1${__Cl};/g
s/\([ \t].*\)_${__cl}/\\\1_${__Cl}/g
s/\([\t(].*\)${__cl}/\\\1${__Cl}/g
s/^\([ \t].*\)${__cl}/\\\1${__Cl}/g" > $__tmpsed
    sed -f $__tmpsed $__in >> $__out
    rm $__tmpsed
}

srcdir=$1
type=`echo $2|sed -e 's/.*\.gir\.\([ch]\)/\1/'`
target=`echo $2|sed -e 's/\.gir\(\.[ch]\)/\1/'`
_cl=`guess_type $target`
_ns=`echo $_cl|cut -d'_' -f1`

_tmpgen=`mktemp gengir.XXXXXXXX`
echo "#include <glib.h>" > $_tmpgen
capitalize "$srcdir/$target" $_tmpgen $_cl

sed -i -e 's,^\(#include[ \t]<\)liblangtag\(/lt-.*\)\(\.h>\),\1liblangtag-gobject\2.gir\3,' $_tmpgen
sed -i -e 's/ssize_t/gssize/g' -e 's/size_t/gsize/g' $_tmpgen

while [ 1 ]; do
    if [ "x$type" = "xh" ]; then
	line=`grep -E "${_ns}_[a-z_].*_t[ \t;)].*" $_tmpgen`
    elif [ "x$type" = "xc" ]; then
	line=`sed -n -e '/\/\*< public >\*\//{:a p;n;b a};{d}' $_tmpgen|grep "[^#]${_ns}_.*_t[^a-z]"|grep -v -E "(func)"`
    else
	echo "Unknown source type: $2"
	exit 1
    fi
    if [ "x$line" = "x" ]; then
	break;
    fi
    _tmpsed=`mktemp gengir.XXXXXXXX`
    printf "/${_ns}_[a-z_].*_t/{s/.*\(${_ns}_[a-z_].*_t\)[^a-z]*/\\\1/;p}\n" > $_tmpsed
    _n=1
    _tt=
    while [ 1 ]; do
	_ll=`echo $line|sed -e 's/[()]/ /g' -e 's/^[ \t].*//' -e 's/[ \t][ \t]*/ /g'|cut -d' ' -f$_n`
	_tt=`echo $_ll|sed -n -f $_tmpsed`
	if [ "x$_ll" = "x" ]; then
	    break
	elif [ "x$_tt" = "x" ]; then
	    _n=`expr $_n + 1`
	else
	    break
	fi
    done
    _cl=$_tt
    rm $_tmpsed
    _in=$_tmpgen
    if [ "x$_cl" = "x" ]; then
	break
    fi
    _tmpgen=`mktemp gengir.XXXXXXXX`
    capitalize $_in $_tmpgen $_cl
    rm $_in
done
mv $_tmpgen $2
