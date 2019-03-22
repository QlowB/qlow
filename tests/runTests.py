#!/usr/bin/python

import sys
import os
import subprocess
import difflib


if len(sys.argv) <= 1:
    print("please specify the qlow executable as a command line argument")
    exit()

qlow_executable = sys.argv[1]

succeeded = 0
failed = 0

def test_file(path):
    test = [qlow_executable, path, "-o", path + ".o"]
    print("running test " + " ".join(test))
    output = subprocess.run(test, stdout=subprocess.PIPE)
    with open(path + ".c.did", "w") as out:
        out.write(output.stdout.decode("utf-8"))
    
    with open(path + ".c.did", "r") as did, open(path + ".c.should", "r") as should:
        if did.readlines() == should.readlines():
            global succeeded
            succeeded += 1
        else:
            global failed
            failed += 1



def run_directory(dir):
    for root, dirs, files in os.walk(dir):
        for filename in files:
            if filename.endswith(".qlw"):
                test_file(os.path.join(root, filename))

def print_results():
    print("%d out of %d tests succeeded: %d%%" % (succeeded, succeeded + failed, 100 * succeeded / (succeeded + failed)))

run_directory(".")

print_results()


