#!/usr/bin/env python
from __future__ import absolute_import, division, print_function
import json
import sys
from collections import OrderedDict
from os.path import abspath, join, normpath, splitext

def main(args):
    """Main entry point allowing external calls

    Args:
        args ([str]): command line parameter list
    """
    idn = "test_graphs"
    odn = "test_results"
    rdn = "test_reference"
    dot_exe = "/usr/bin/dot"

    tfn = "TESTS.json"

    layout = "dot"
    with open(tfn, 'r') as ifh:
        testgroup = json.load(ifh, object_pairs_hook=OrderedDict)

    ict = 0
    for testlabel, testitem in testgroup.items():
        ict += 1
        print("Test {0:03d}:".format(ict))
        print("  Label: {0:s}".format(testlabel))
        print("  Code:  {0:s}".format(testitem["code"]))
        print("  Subtest(s):")
        graphfn = join(idn, "{0:03d}.{1:s}".format(ict, "gv"))
        headcmt = "// {0:03d}: {1:s}".format(ict, testlabel)
        # Write headcmt followed by testitem["code"] to graphfn
        # TODO: \-escape testlabel or write raw/verbatim

        jct = 0
        for tfmt in testitem["format"]:
            jct += 1
            basefn = "{0:03d}.{1:03d}".format(ict, jct)
            ifn = join(idn, "{0:s}.{1:s}".format(basefn, "gv"))
            ofn = join(odn, "{0:s}.{1:s}".format(basefn, tfmt))
            rfn = join(rdn, "{0:s}.{1:s}".format(basefn, tfmt))
            cmd = "{0:s} -K{1:s} -T{2:s} {3:s} -o {4:s}" \
                  .format(dot_exe, layout, tfmt, ifn, ofn)
            print("    {0:03d}.{1:03d}: {2:s}"
                  .format(ict, jct, cmd))

        print()

    return


def run():
    """Entry point for console_scripts
    """
    main(sys.argv[1:])
    return


if __name__ == "__main__":
    run()
