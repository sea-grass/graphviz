#!/usr/bin/env python
"""
Test generator and runner for detecting differences in text and images produced by dot

Directions for use:
- Create reference directory and populate it with reference data.
- Create "test roster", a JSON-formatted file consisting of a
  dict of test definitions. Default name is "TESTS.json"
  - key is test label
  - value is a dict
    - required key "graph", value is a string, the backslash-escaped 
      gv commands to run through dot
    - required key "format", value is a list of strings which can
      be accepted by dot's -T option. At least one format must be
      listed; list cannot be empty
    - any other keys are ignored
  - Optionally, create input and output directories which are
    writable by this script.
- Run the script, specifying the path to the dot executable, the test
  definition file, and the reference directory, and any optional
  parameters (input and output directories, default layout)

Interpreting results:
- Output is in TAP format; see https://testanything.org/
  - The range of subtests for a given test is displayed as "1..<number of subtests>"
  - Each subtest will display either "ok" or "not ok" followed by the
    subtest ID and a descriptive message on stdout.
- The test script exits with an integer status code:
  - If all tests succeed, the script exits with status code 0.
  - If any subtest fails, the script exits with status code 1
  - If no tests can run (usually due to misconfiguration),
    the script exits with status code 2
- For failing text comparisons, a unified diff of the results and
  reference files is produced in the results directory. Text
  difference files are named <test>.<subtest>.<format>.diff
- For failing image comparisons, an image difference file is created
  from the results and reference image files in the results
  directory. Image difference files are named
  <test>.<subtest>.diff.<format>
"""
from __future__ import absolute_import, division, print_function
import argparse
from collections import defaultdict, OrderedDict
from difflib import unified_diff
from filecmp import cmp
import json
from os import mkdir, remove
from os.path import abspath, exists, join
from PIL import Image
from pixelmatch.contrib.PIL import pixelmatch
import pytest
import subprocess
import sys

# Avoid caching .pyc bytecode to disk
sys.dont_write_bytecode = True

# Global results comparison selector
# Ensure any format that appears in TESTS.json is categorized as
# "text" or "image". Unlisted formats will be categorized as
# "unknown" by default and no detailed difference checks will occur
fmttest = defaultdict(lambda : "unknown")
for rt in [ "png" ]:
    fmttest[rt] = "image"
for rt in [ "cmapx", "gv", "ps", "svg", "xdot" ]:
    fmttest[rt] = "text"


def parse_args(args):
    """Parse command line parameters

    Args:
      args ([str]): command line parameters as list of strings

    Returns:
      :obj:`argparse.Namespace`: command line parameters namespace
    """
    parser = argparse.ArgumentParser(
        description="rtest2 test runner")
    parser.add_argument(
        "-t",
        "--test-definitions",
        dest="test_json",
        help="set JSON test definition file",
        default=join(abspath("."), "TESTS.json"),
        metavar="TESTS_JSON",
        action="store")
    parser.add_argument(
        "-x",
        "--dot-executable",
        dest="dot_exe",
        help="set dot executable",
        default=join(abspath("."), "dot"),
        metavar="DOT_EXECUTABLE",
        action="store")
    parser.add_argument(
        "-i",
        "--input-dir",
        dest="graph_dir",
        help="set directory to contain generated input files",
        default=join(abspath("."), "test_graphs"),
        action="store")
    parser.add_argument(
        "-o",
        "--output-dir",
        dest="results_dir",
        help="set directory to contain generated results",
        default=join(abspath("."), "test_results"),
        action="store")
    parser.add_argument(
        "-r",
        "--reference-dir",
        dest="reference_dir",
        help="set directory containing reference results",
        default=join(abspath("."), "test_reference"),
        action="store")
    parser.add_argument(
        "-L",
        "--layout",
        dest="layout",
        help="set layout engine",
        default="dot",
        action="store")
    parser.add_argument(
        "-T",
        "--no-tap",
        dest="write_tapfiles",
        help="do not write .tap test output files",
        default=True,
        action="store_false")
    parser.add_argument(
        "-P",
        "--no-pytest",
        dest="use_pytest",
        help="do not run pytest assert()s",
        default=True,
        action="store_false")
    parser.add_argument(
        "-q",
        "--quiet",
        dest="write_stdout",
        help="do not write test output to stdout",
        default=True,
        action="store_false")

    return parser.parse_args(args)


def get_tap_filename(results_dir, test_id):
    """Generate the full path to a TAP results file given a results directory
    and a test ID number. If the results directory does not exist, 
    the current working directory is used.

    Args:
        results_dir [str]: test results directory
        test_id [int]: test ID number

    Returns:
        str: Full path of TAP results file
    """
    tap_dir = "."
    if exists(results_dir):
        tap_dir = results_dir

    return abspath(join(tap_dir, "test_{0:d}.tap".format(test_id)))


def add_tap_plan(tap_output):
    """Infer the number tests run from a list of TAP-formated test results
    and prepend 'plan' (test count formatted as "1..#" where # is the ID
    number of the last test)

    Args:
        tap_output ([str]): list of test results in Test Anything Protocol
                            format
    """
    ntests = len(tap_output)
    if ntests > 0:
        # Prepend plan to tap_output list
        tap_output.insert(0, "1..{0:d}\n".format(ntests))


def write_tap(results_dir, test_id, tap_output):
    """Write TAP-formatted test results to file

    Args:
        results_dir [str]: test results directory
        test_id [int]: test ID number
        tap_output ([str]): list of test results in Test Anything Protocol
                            format, including plan. List entries should be
                            terminated with a newline
    """
    if len(tap_output) > 0:
        tap_fn = get_tap_filename(results_dir, test_id)
        with open(tap_fn, "w") as tapfh:
            tapfh.writelines(tap_output)


def test_rtest2(
    arg_dot_exe,
    arg_reference_dir,
    arg_test_json="TESTS.json",
    arg_layout="dot",
    arg_graph_dir="./test_graphs",
    arg_results_dir="./test_results",
    arg_write_stdout=True,
    arg_write_tapfiles=True,
    arg_use_pytest=True):
    """Main entry point allowing external calls

    Args:
        arg_dot_exe (str): location of dot executable
        arg_reference_dir (str): path of reference directory
        arg_test_json (str): location of test definition file
        arg_layout (str): name of layout engine
        arg_graph_dir (str): path of input graph directory
        arg_results_dir (str): path of output results directory
        arg_write_stdout (bool): flag enabling writing results to stdout
        arg_write_tapfiles (bool): flag indicating TAP results should be written to file
        arg_use_pytest (bool): flag indicating pytest assert()s should be executed

    Returns:
        int: Status code - 0 = success, non-zero = failure
    """

    # Overall test suite status: zero indicates success and non-zero indicates failure
    # Initiallize to 0, set to 1 if any test or subtest fails, set to 2 if no tests can be run
    iok = 0

    test_id = 0
    subtest_id = 0
    tap_output = []

    # The following are persistent variables set during the pretest
    test_group = OrderedDict()
    dot_exe = ''
    graph_dir = ''
    results_dir = ''
    reference_dir = ''
    layout = ''

    # Check for dot executable
    subtest_id += 1
    subtest_label = "Test {0:d}.{1:d}: Dot executable".format(test_id, subtest_id)
    if exists(arg_dot_exe):
        dot_exe = abspath(arg_dot_exe)
        subtest_status = "ok"
        subtest_comment = " # Found {0:s}".format(dot_exe)
    else:
        subtest_status = "not ok"
        subtest_comment = " # Cannot find {0:s}".format(arg_dot_exe)

    if subtest_status != "ok":
        iok = 2
    tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
        .format(subtest_status, subtest_id, subtest_label, subtest_comment)
    tap_output.append(tap_entry + "\n")
    if arg_use_pytest:
        assert subtest_status == "ok", tap_entry

    # Check for test definitions
    subtest_id += 1
    subtest_label = "Test {0:d}.{1:d}: Test definitions".format(test_id, subtest_id)
    if exists(arg_test_json):
        tfn = abspath(arg_test_json)
        # Import test roster from JSON input
        with open(tfn, 'r') as ifh:
            test_group = json.load(ifh, object_pairs_hook=OrderedDict)

        ntests = len(test_group)
        if ntests > 0:
            subtest_status = "ok"
            subtest_comment = " # Found {0:d} tests in {1:s}".format(ntests, tfn)
        else:
            subtest_status = "not ok"
            subtest_comment = " # No tests found in {0:s} (is it JSON?)".format(tfn)
    else:
        subtest_status = "not ok"
        subtest_comment = " # Cannot find {0:s}".format(arg_test_json)

    if subtest_status != "ok":
        iok = 2
    tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
        .format(subtest_status, subtest_id, subtest_label, subtest_comment)
    tap_output.append(tap_entry + "\n")
    if arg_use_pytest:
        assert subtest_status == "ok", tap_entry

    # Check if input directory exists or can be created
    subtest_id += 1
    subtest_label = "Test {0:d}.{1:d}: Input directory".format(test_id, subtest_id)
    graph_dir = abspath(arg_graph_dir)
    if not exists(graph_dir):
        mkdir(graph_dir)
    if exists(graph_dir):
        subtest_status = "ok"
        subtest_comment = " # Found {0:s}".format(tfn)
    else:
        subtest_status = "not ok"
        subtest_comment = " # Cannot create {0:s}".format(arg_graph_dir)

    if subtest_status != "ok":
        iok = 2
    tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
        .format(subtest_status, subtest_id, subtest_label, subtest_comment)
    tap_output.append(tap_entry + "\n")
    if arg_use_pytest:
        assert subtest_status == "ok", tap_entry

    # Check if results directory exists or can be created
    subtest_id += 1
    subtest_label = "Test {0:d}.{1:d}: Results directory".format(test_id, subtest_id)
    results_dir = abspath(arg_results_dir)
    if not exists(results_dir):
        mkdir(results_dir)
    if exists(results_dir):
        subtest_status = "ok"
        subtest_comment = " # Found {0:s}".format(results_dir)
    else:
        subtest_status = "not ok"
        subtest_comment = " # Cannot create {0:s}".format(arg_results_dir)

    if subtest_status != "ok":
        iok = 2
    tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
        .format(subtest_status, subtest_id, subtest_label, subtest_comment)
    tap_output.append(tap_entry + "\n")
    if arg_use_pytest:
        assert subtest_status == "ok", tap_entry

    # Check if reference directory exists
    subtest_id += 1
    subtest_label = "Test {0:d}.{1:d}: Reference directory".format(test_id, subtest_id)
    if exists(arg_reference_dir):
        reference_dir = abspath(arg_reference_dir)
        subtest_status = "ok"
        subtest_comment = " # Found {0:s}".format(reference_dir)
    else:
        subtest_status = "not ok"
        subtest_comment = " # Cannot find {0:s}".format(arg_reference_dir)

    if subtest_status != "ok":
        iok = 2
    tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
        .format(subtest_status, subtest_id, subtest_label, subtest_comment)
    tap_output.append(tap_entry + "\n")
    if arg_use_pytest:
        assert subtest_status == "ok", tap_entry

    # Check if layout engine is defined
    subtest_id += 1
    subtest_label = "Test {0:d}.{1:d}: Layout engine".format(test_id, subtest_id)
    layout = arg_layout.strip().lower()
    if len(layout) > 0:
        subtest_status = "ok"
        subtest_comment = " # Setting to {0:s}".format(layout)
    else:
        subtest_status = "not ok"
        subtest_comment = " # Layout engine not specified"

    if subtest_status != "ok":
        iok = 2
    tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
        .format(subtest_status, subtest_id, subtest_label, subtest_comment)
    tap_output.append(tap_entry + "\n")
    if arg_use_pytest:
        assert subtest_status == "ok", tap_entry

    # Remove stale TAP file
    tap_fn = get_tap_filename(results_dir, 0)
    if exists(tap_fn):
        remove(tap_fn)

    add_tap_plan(tap_output)
    if arg_write_stdout:
        sys.stdout.writelines(tap_output)
    if arg_write_tapfiles:
        write_tap(results_dir, 0, tap_output)

    # Quit if pretests fail
    if iok != 0:
        return iok

    # Generate and run tests
    for test_label, test_item in test_group.items():
        test_id += 1

        # Wipe stale TAP output
        tap_output = []

        tap_fn = get_tap_filename(results_dir, test_id)
        if exists(tap_fn):
            remove(tap_fn)

        # Common input file to all subtests of test test_id
        # Write headcmt followed by test_item["code"] to graph_fn
        graph_fn = join(graph_dir, "{0:03d}.{1:s}".format(test_id, "gv"))
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
            layout_opt = "-K{0:s}".format(layout)
            format_opt = "-T{0:s}".format(subtest_format)
            cmd_args = [dot_exe, layout_opt, format_opt, graph_fn, "-o", results_fn]

            # Run the test
            pstatus = subprocess.run(cmd_args)

            # if (pstatus.returncode != 0):
            #     _logger.info("Subtest {0:s} errored out with status code {1:d}:"
            #         .format(tag, pstatus.returncode))
            # else:
            #     _logger.info("Subtest {0:s} completed successfully"
            #         .format(tag))

            # if (pstatus.stdout):
            #     _logger.info("stdout: {0:s}".format(pstatus.stdout))

            # if (pstatus.stderr):
            #     _logger.info("stderr: {0:s}".format(pstatus.stderr))

            # Analyze results
            if exists(results_fn) and exists(reference_fn):
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

            # TAP output: Generate subtest results
            tap_entry = "{0:s} {1:d} - {2:s}{3:s}" \
                .format(subtest_status, subtest_id, subtest_label, subtest_comment)
            tap_output.append(tap_entry + "\n")
            if arg_use_pytest:
                assert subtest_status == "ok", tap_entry

        add_tap_plan(tap_output)
        if arg_write_stdout:
            sys.stdout.writelines(tap_output)
        if arg_write_tapfiles:
            write_tap(results_dir, test_id, tap_output)

    return iok


def run():
    """Entry point for console_scripts

    Returns:
        int: Status code - 0 = success, non-zero = failure
    """
    args = parse_args(sys.argv[1:])
    iok = test_rtest2(
        arg_dot_exe=args.dot_exe,
        arg_reference_dir=args.reference_dir,
        arg_test_json=args.test_json,
        arg_layout=args.layout,
        arg_graph_dir=args.graph_dir,
        arg_results_dir=args.results_dir,
        arg_write_stdout=args.write_stdout,
        arg_write_tapfiles=args.write_tapfiles,
        arg_use_pytest=args.use_pytest)

    return iok


if __name__ == "__main__":
    iok = run()
    sys.exit(iok)
