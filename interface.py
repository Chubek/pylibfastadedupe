from typing import List, Tuple
from Bio.SeqIO import parse
from Bio import SeqRecord
import json
import argparse
import gzip
import io
from time import time
import cffi

ffi = cffi.FFI()

ffi.cdef("""
    extern void pylibfastadedupe_ffi(char *seqs[], int size[1], int out[], int out_revcomp[], int num_threads[1], int num_workers[1], int do_revcomp[1]);
""")

lib = ffi.dlopen("./_libfastadedupe.so")

NULL_CHAR = '\0'.encode('ascii')

def read_fasta_file(path: str) -> Tuple[int, List[bytearray], List[Tuple[str, str]]]:
    input_file = open(path, "r")
    path_split = path.split(".")

    if path_split[-1] == "fasta" or path_split[-1] == "fa":
        records = parse(input_file, "fasta")
    elif path_split[-1] == "gz":
        with gzip.open("path") as gzf:
            records_text = gzf.read()

            io_ = io.stringIO(records_text)
            
            if path_split[-2] == "fasta" or path_split[-2] == "fa":
                records = parse(io_, "fasta")
            elif path_split[-2] == "fastq" or path_split[-2] == "fastaq":
                records = parse(io_, "fastq")
            else:
                raise Exception("Gzip file is not fastaq, fastq or fasta")
    elif path_split[-1] == "fastq" or path_split[-1] == "fastaq":
        records = parse(input_file, "fastq")

    else:
        raise Exception("File is not fastaq, fastq or fasta, nor gzip of them")

    i = [0]
    bytes_arr = []
    def parse_records(record: SeqRecord, bytes_arr=bytes_arr):
        header = f"|{record.id}|"
        seq = record.seq
        i[0] += 1
        bytes_arr.append(ffi.new("char[]", (seq.encode('ascii') + NULL_CHAR)))
        return (header, seq)


    records_parsed = list(map(parse_records, records))

    return i[0], bytes_arr, records_parsed


def run_pylibfastadedupe(path: str, num_threads=32, num_workers=256, do_revcomp=False):
    t = time()
    size, seqs, records = read_fasta_file(path)
    print(f"Records parsed, took {time() - t} seconds")
    
    out = ffi.new("int[]", [-1] * size)
    out_revcomp = ffi.new("int[]", [-1] * size)
    num_threads_ = ffi.new("int[]", [num_threads])
    num_workers_ = ffi.new("int[]", [num_workers])
    do_revcomp_ = ffi.new("int[]", [int(do_revcomp)])
    size_ = ffi.new("int[]", [size])

    print(f"Running pylibfastadedupe with {num_threads} main threads, {num_workers} worker threads and {'not' if do_revcomp else ''} doing revcomp")
    t = time()
    lib.pylibfastadedupe_ffi(seqs, size_, out, out_revcomp, num_threads_, num_workers_, do_revcomp_)
    print(f"Dedupe successfully done, toook {time() - t} seconds")


if __name__ == "__main__":
    argp = argparse.ArgumentParser()
    argp.add_argument("-i", "--input", type=str, required=True)
    argp.add_argument("-rc", "--do_revcomp", action="store_true")
    argp.add_argument("-w", "--workers", type=int, default=256)
    argp.add_argument("-p", "--threads", type=int, default=32)

    args = argp.parse_args()


    run_pylibfastadedupe(args.input, args.threads, args.workers, args.do_revcomp)

    ffi.dlclose(lib)