num_threads=32
num_workers=256
file=$DEDUPE_FILE
do_revcomp=''
debug=''
while getopts rc?p:w:f:d? flag
do
    case "${flag}" in
        rc) rc='-rc';;
        f) file=${OPTARG};;
        w) num_workers=${OPTARG};;
        p) num_threads=${OPTARG};;
        d) debug='gdb -ex r --args';;
    esac
done

$debug python3 interface.py -i $file $rc -w $num_workers -p $num_threads