#!/usr/bin/env python
from __future__ import absolute_import, division, print_function
from collections import defaultdict, OrderedDict
from difflib import unified_diff
from filecmp import cmp
import json
# import logging
from os import mkdir, remove
from os.path import abspath, exists, join
from PIL import Image
from pixelmatch.contrib.PIL import pixelmatch
import subprocess
import sys

# Global results comparison selector
fmttest = defaultdict(lambda : "unknown")
for rt in [ "png" ]:
    fmttest[rt] = "image"
for rt in [ "cmapx", "gv", "ps", "svg", "xdot" ]:
    fmttest[rt] = "text"

def main(args):
    """Main entry point allowing external calls

    Args:
        args ([str]): command line parameter list
    """
    # Note: dot_exe, tfn, and layout should be specifiable from the command line
    # Also, either (basedn, idn, odn, rdn) or (graph_dir, results_dir, reference_dir)
    # should be specificiable so CMake/CTest can run this as a test
    basedn = "."
    idn = "test_graphs"
    odn = "test_results"
    rdn = "test_reference"
    dot_exe = "dot"
#    dot_exe = "/usr/bin/dot"
#    dot_exe = r"C:\Program Files (x86)\Graphviz2.38\bin\dot.exe"

    tfn = "TESTS.json"

    layout = "dot"

    # Compatibility flag: set to False for original test.tcl behavior
    use_common_input = True

    is_verbose = True

    with open(tfn, 'r') as ifh:
        testgroup = json.load(ifh, object_pairs_hook=OrderedDict)

    graph_dir = abspath(join(basedn, idn))
    if not exists(graph_dir):
        mkdir(graph_dir)

    results_dir = abspath(join(basedn, odn))
    if not exists(results_dir):
        mkdir(results_dir)

    reference_dir = abspath(join(basedn, rdn))
    if not exists(reference_dir):
        print("ERROR: Reference directory {0:s} does not exist".format(reference_dir))
        exit

    ict = 0
    for testlabel, testitem in testgroup.items():
        ict += 1
        if is_verbose:
            print("Test {0:03d}:".format(ict))
            print("  Label: {0:s}".format(testlabel))
            print("  Code:  {0:s}".format(testitem["code"]))
            if len(testitem["format"]) == 1:
                print("  Subtest:")
            else:
                print("  Subtests:")

        # Common input file to all subtests of test ict
        # Write headcmt followed by testitem["code"] to graph_fn
        graph_fn = join(graph_dir, "{0:03d}.{1:s}".format(ict, "gv"))
        if use_common_input:
            if exists(graph_fn):
                # Wipe stale input
                remove(graph_fn)
            headcmt = "// {0:03d}: {1:s}\n".format(ict, testlabel)
            with open(graph_fn, 'w') as ofh:
                ofh.write(headcmt)
                ofh.write(testitem["code"] + "\n")

        jct = 0
        for tfmt in testitem["format"]:
            jct += 1
            basefn = "{0:03d}.{1:03d}".format(ict, jct)

            # Unique input file for subtest ict.jct
            # Write head comments followed by testitem["code"] to subgraph_fn
            # Unique input file for each subtest of test #ict (original behavior)
            if not use_common_input:
                subgraph_fn = join(graph_dir, "{0:s}.{1:s}".format(basefn, "gv"))
                if exists(subgraph_fn):
                    # Wipe stale input
                    remove(subgraph_fn)
                headcmt1 = "// {0:03d}.{1:03d}: {2:s}\n".format(ict, jct, testlabel)
                headcmt2 = "// dot -T{0:s}\n".format(tfmt)
                with open(subgraph_fn, 'w') as ofh:
                    ofh.write(headcmt1)
                    ofh.write(headcmt2)
                    ofh.write(testitem["code"] + "\n")

            # Output file
            results_fn = join(results_dir, "{0:s}.{1:s}".format(basefn, tfmt))
            if exists(results_fn):
                # Wipe stale output
                remove(results_fn)

            # Reference file
            reference_fn = join(reference_dir, "{0:s}.{1:s}".format(basefn, tfmt))

            # Compose command and arguments as list
            cmd_args = [dot_exe]
            cmd_args.append("-K" + layout)
            cmd_args.append("-T" + tfmt)
            if use_common_input:
                # Use common test input file
                cmd_args.append(graph_fn)
            else:
                # Original behavior
                cmd_args.append(subgraph_fn)

            cmd_args.append("-o")
            cmd_args.append(results_fn)

            # Display command
            if is_verbose:
                cmd = " ".join(cmd_args)
                print("    {0:s}: {1:s}".format(basefn, cmd))

            pstatus = subprocess.run(cmd_args)
            if (pstatus.returncode != 0):
                print("Subtest {0:s} errored out with status code {1:d}:"
                      .format(basefn, pstatus.returncode))
            else:
                print("Subtest {0:s} completed successfully"
                      .format(basefn))

            if is_verbose:
                if (pstatus.stdout):
                    print("stdout: {0:s}".format(pstatus.stdout))

                if (pstatus.stderr):
                    print("stderr: {0:s}".format(pstatus.stderr))

            if exists(results_fn) and exists(reference_fn):
                if is_verbose:
                    print("Found reference file {0:s} corresponding to results file {1:s}"
                        .format(reference_fn, results_fn))
                # TODO Perform all results comparisons and tests here
                if cmp(reference_fn, results_fn):
                    print("OK - Results from subtest {0:s} are trivially similar".format(basefn))
                else:
                    if fmttest[tfmt] == "text":
                        print("FAIL - Text results from subtest {0:s} differ from reference:".format(basefn))
                        if is_verbose:
                            with open(reference_fn) as ff:
                                reference_data = ff.readlines()
                            with open(results_fn) as tf:
                                results_data = tf.readlines()
                            textdiff = unified_diff(reference_data, results_data, reference_fn, results_fn)
                            sys.stdout.writelines(textdiff)
                    elif fmttest[tfmt] == "image":
                        # Compare images
                        ref_img = Image.open(reference_fn)
                        results_img = Image.open(results_fn)
                        # TODO: Add simple histogram checks
                        if ref_img.size != results_img.size:
                            print("FAIL - Subtest {0:s} image size ({1:d}, {2:d}) differs from reference size ({3:d}, {4:d})"
                                  .format(basefn, results_img.size[0], results_img.size[1], ref_img.size[0], ref_img.size[1], ))
                        else:
                            img_diff = Image.new("RGBA", ref_img.size)
                            idiff_fn = join(results_dir, "{0:s}.diff.{1:s}".format(basefn, tfmt))
                            mismatch = pixelmatch(ref_img, results_img, img_diff, includeAA=True)
                            img_diff.save(idiff_fn)
                            if mismatch > 0:
                                print("FAIL - Image results from subtest {0:s} differ from reference by {1:d} pixels"
                                    .format(basefn, mismatch))
                            else:
                                print("FAIL - Image results from subtest {0:s} differ from reference but pixels seem to match (alpha channel?)"
                                    .format(basefn))
                    else:
                        print("FAIL - Unknown format results from subtest {0:s} differ from reference:".format(basefn))

            else:            
                # TODO Report subtest skip or failure
                print("FAIL - Subtest {0:s} did not produce any results".format(basefn))

        print()

    return


def run():
    """Entry point for console_scripts
    """
    main(sys.argv[1:])
    return


if __name__ == "__main__":
    run()
