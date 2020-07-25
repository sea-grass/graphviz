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

# Directions for use:
# - Create reference directory and populate it with reference data.
#   See basedn, rdn, and reference_dir in script body
# - Create "test roster", a JSON-formatted file consisting of a
#   dict of test definitions. Default name is "TESTS.json"
#   - key is test label
#   - value is a dict
#     - required key "graph", value is a string, the \-escaped 
#       gv commands to run through dot
#     - required key "format", value is a list of strings which can
#       be accepted by dot's -T option. At least one format must be
#       listed; list cannot be empty
#     - any other keys are ignored
#   - Optionally, create input and output directories which are
#     writable by this script. See basedn, idn, graph_dir, odn,
#     and results_dir in script body
# - Run the script, specifying the full path to the dot executable,
#   the input, output, and reference directories, and any optional
#   parameters (verbosity, default layout)
#
# Interpreting results:
# - Each subtest will display either "OK" or "FAIL" followed by the
#   test.subtest ID and a descriptive message on stdout.
#   - If all tests succeed, the script exits with status code 0.
#   - If any subtest fails, the script exits with status code 1
#   - If no tests can run (usually due to missing reference data),
#     the script exits with status code 2
# - For failing text comparisons, a unified diff of the results and
#   reference files is produced in the results directory. Text
#   difference files are named <test>.<subtest>.<format>.diff
# - For failing image comparisons, an image difference file is created
#   from the results and reference image files in the results
#   directory. Image difference files are named
#   <test>.<subtest>.diff.<format>

# Global results comparison selector
# Ensure any format that appears in TESTS.json is categorized as
# "text" or "image". Unlisted formats will be categorized as
# "unknown" by default and no detailed difference checks will occur
fmttest = defaultdict(lambda : "unknown")
for rt in [ "png" ]:
    fmttest[rt] = "image"
for rt in [ "cmapx", "gv", "ps", "svg", "xdot" ]:
    fmttest[rt] = "text"

def main(args):
    """Main entry point allowing external calls

    Args:
        args ([str]): command line parameter list

    Returns:
        int: Status code - 0 = success, non-zero = failure
    """
    # Note: dot_exe, tfn, and layout should be specifiable from the command line
    dot_exe = "dot"
#    dot_exe = "/usr/bin/dot"
#    dot_exe = r"C:\Program Files (x86)\Graphviz2.38\bin\dot.exe"

    tfn = "TESTS.json"

    layout = "dot"

    # Also, either (basedn, idn, odn, rdn) or (graph_dir, results_dir, reference_dir)
    # should be specificiable so CMake/CTest can run this as a test
    basedn = "."
    idn = "test_graphs"
    odn = "test_results"
    rdn = "test_reference"

    graph_dir = abspath(join(basedn, idn))
    results_dir = abspath(join(basedn, odn))
    reference_dir = abspath(join(basedn, rdn))

    # Compatibility flag: set to False for original test.tcl behavior
    use_common_input = True

    is_verbose = False

    # Overall test suite status: zero indicates success and non-zero indicates failure
    # Initiallize to 0, set to 1 if any test or subtest fails, set to 2 if no tests can be run
    iok = 0

    # Fail if reference data directory cannot be found
    print("1..1")
    if exists(reference_dir):
        print("ok 1 - Test 0: Reference directory check # Found {0:s}".format(reference_dir))
    else:
        print("not ok 1 - Test 0: Reference directory check # Cannot find {0:s}".format(reference_dir))
        iok = 2
        return iok

    # Build input and output directories if they don't already exist
    for tdn in [graph_dir, results_dir]:
        if not exists(tdn):
            mkdir(tdn)

    # Import test roster from JSON input
    with open(tfn, 'r') as ifh:
        test_group = json.load(ifh, object_pairs_hook=OrderedDict)

    # Generate and run tests
    test_id = 0
    for test_label, test_item in test_group.items():
        test_id += 1
        nsubtests = len(test_item["format"])
        # TAP output: Print range of subtests
        print("1..{0:d}".format(nsubtests))
        if is_verbose:
            print("Test {0:03d}:".format(test_id))
            print("  Label: {0:s}".format(test_label))
            print("  Code:  {0:s}".format(test_item["code"]))
            if len(test_item["format"]) == 1:
                print("  Subtest:")
            else:
                print("  Subtests:")

        # Common input file to all subtests of test test_id
        # Write headcmt followed by test_item["code"] to graph_fn
        graph_fn = join(graph_dir, "{0:03d}.{1:s}".format(test_id, "gv"))
        if use_common_input:
            # Wipe stale input
            if exists(graph_fn):
                remove(graph_fn)
            headcmt = "// {0:03d}: {1:s}\n".format(test_id, test_label)
            with open(graph_fn, 'w') as ofh:
                ofh.write(headcmt)
                ofh.write(test_item["code"] + "\n")

        subtest_id = 0
        for subtest_format in test_item["format"]:
            # Initialize TAP subtest fields
            subtest_id += 1
            subtest_status = "ok"
            subtest_label = "Test {0:d}: {1:s}, format = {2:s}" \
                .format(test_id, test_label, subtest_format)
            subtest_comment = ''

            tag = "{0:03d}.{1:03d}".format(test_id, subtest_id)

            # Unique input file for subtest test_id.subtest_id
            # Write head comments followed by test_item["code"] to subgraph_fn
            # Unique input file for each subtest of test #test_id (original behavior)
            if not use_common_input:
                subgraph_fn = join(graph_dir, "{0:s}.{1:s}".format(tag, "gv"))
                # Wipe stale input
                if exists(subgraph_fn):
                    remove(subgraph_fn)
                headcmt1 = "// {0:03d}.{1:03d}: {2:s}\n".format(test_id, subtest_id, test_label)
                headcmt2 = "// dot -T{0:s}\n".format(subtest_format)
                with open(subgraph_fn, 'w') as ofh:
                    ofh.write(headcmt1)
                    ofh.write(headcmt2)
                    ofh.write(test_item["code"] + "\n")

            # Output file
            results_fn = join(results_dir, "{0:s}.{1:s}".format(tag, subtest_format))
            # Wipe stale output
            if exists(results_fn):
                remove(results_fn)

            # Reference file
            reference_fn = join(reference_dir, "{0:s}.{1:s}".format(tag, subtest_format))
            # Note: If reference_fn doesn't exist, we could short-circuit this test
            # and fail on lack of reference data. Probably better to run the test
            # anyway to generate results, providing reference data for subsequent runs

            # Compose command and arguments as list
            cmd_args = [dot_exe]
            cmd_args.append("-K" + layout)
            cmd_args.append("-T" + subtest_format)
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
                print("    {0:s}: {1:s}".format(tag, cmd))

            # Run the test
            pstatus = subprocess.run(cmd_args)

            if is_verbose:
                if (pstatus.returncode != 0):
                    print("Subtest {0:s} errored out with status code {1:d}:"
                        .format(tag, pstatus.returncode))
                else:
                    print("Subtest {0:s} completed successfully"
                        .format(tag))

                if (pstatus.stdout):
                    print("stdout: {0:s}".format(pstatus.stdout))

                if (pstatus.stderr):
                    print("stderr: {0:s}".format(pstatus.stderr))

            # Analyze results
            if exists(results_fn) and exists(reference_fn):
                if is_verbose:
                    print("Found reference file {0:s} corresponding to results file {1:s}"
                        .format(reference_fn, results_fn))
                if cmp(reference_fn, results_fn):
                    subtest_comment = " # Results are trivially similar"

                else:
                    if fmttest[subtest_format] == "text":
                        iok = 1
                        subtest_status = "not ok"
                        with open(reference_fn) as ff:
                            reference_data = ff.readlines()
                        with open(results_fn) as tf:
                            results_data = tf.readlines()
                        textdiff = unified_diff(reference_data, results_data, reference_fn, results_fn)
                        tdiff_fn = join(results_dir, "{0:s}.{1:s}.diff".format(tag, subtest_format))
                        with open(tdiff_fn, "w") as tdf:
                            tdf.writelines(textdiff)

                        subtest_comment = " # Text results differ from reference; see {0:s}".format(tdiff_fn)

                    elif fmttest[subtest_format] == "image":
                        # Compare images
                        ref_img = Image.open(reference_fn)
                        results_img = Image.open(results_fn)
                        # TODO: Add simple histogram checks
                        if ref_img.size != results_img.size:
                            iok = 1
                            subtest_status = "not ok"
                            subtest_comment = " # Image size ({0:d}, {1:d}) differs from reference size ({2:d}, {3:d})" \
                                .format(results_img.size[0], results_img.size[1], ref_img.size[0], ref_img.size[1])
                        else:
                            img_diff = Image.new("RGBA", ref_img.size)
                            idiff_fn = join(results_dir, "{0:s}.diff.{1:s}".format(tag, subtest_format))
                            mismatch = pixelmatch(ref_img, results_img, img_diff, includeAA=True)
                            img_diff.save(idiff_fn)
                            if mismatch > 0:
                                iok = 1
                                subtest_status = "not ok"
                                subtest_comment = " # Image results differ from reference by {0:d} pixels; see {1:s}" \
                                        .format(mismatch, idiff_fn)
                            else:
                                iok = 1
                                subtest_status = "not ok"
                                subtest_comment = " # Image results differ from reference but pixels seem to match (alpha channel?); see {0:s}" \
                                    .format(idiff_fn)
                    else:
                        iok = 1
                        subtest_status = "not ok"
                        subtest_comment = " # Unknown format results from differ from reference (add {0:s} to fmttest)" \
                            .format(subtest_format)

            else:            
                iok = 1
                subtest_status = "not ok"
                subtest_comment = " # Subtest did not produce any results"

            # TAP output: Print subtest results
            print("{0:s} {1:d} - {2:s}{3:s}"
                .format(subtest_status, subtest_id, subtest_label, subtest_comment))

        if is_verbose:
            print()

    return iok


def run():
    """Entry point for console_scripts

    Returns:
        int: Status code - 0 = success, non-zero = failure
    """
    iok = main(sys.argv[1:])
    return iok


if __name__ == "__main__":
    iok = run()
    sys.exit(iok)
