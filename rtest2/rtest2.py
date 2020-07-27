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
  parameters (verbosity, input and output directories, default layout)

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
import logging
from os import mkdir, remove
from os.path import abspath, exists, join
from PIL import Image
from pixelmatch.contrib.PIL import pixelmatch
import subprocess
import sys

# Set default logging handler to avoid "No handler found" warnings.
try:  # Python 2.7+
    from logging import NullHandler
except ImportError:
    class NullHandler(logging.Handler):
        def emit(self, record):
            pass

_logger = logging.getLogger(__name__)
_logger.addHandler(NullHandler())

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
        '-t',
        '--test-definitions',
        dest="test_json",
        help="set JSON test definition file",
        default=join(abspath("."), "TESTS.json"),
        metavar="TESTS_JSON",
        action='store')
    parser.add_argument(
        '-x',
        '--dot-executable',
        dest="dot_exe",
        help="set dot executable",
        default=join(abspath("."), "dot"),
        metavar="DOT_EXECUTABLE",
        action='store')
    parser.add_argument(
        '-i',
        '--input-dir',
        dest="graph_dir",
        help="set directory to contain generated input files",
        default=join(abspath("."), "test_graphs"),
        action='store')
    parser.add_argument(
        '-o',
        '--output-dir',
        dest="results_dir",
        help="set directory to contain generated results",
        default=join(abspath("."), "test_results"),
        action='store')
    parser.add_argument(
        '-r',
        '--reference-dir',
        dest="reference_dir",
        help="set directory containing reference results",
        default=join(abspath("."), "test_reference"),
        action='store')
    parser.add_argument(
        '-L',
        '--layout',
        dest="layout",
        help="set layout engine",
        default="dot",
        action='store')
    parser.add_argument(
        '-T',
        '--tap',
        dest="write_tapfiles",
        help="write test output in TAP format (Test Anything Protocol)",
        default=False,
        action='store_true')
    parser.add_argument(
        '-q',
        '--quiet',
        dest="write_stdout",
        help="do not write test output to stdout",
        default=True,
        action='store_false')
    parser.add_argument(
        '-v',
        '--verbose',
        dest="loglevel",
        help="set loglevel to INFO",
        action='store_const',
        const=logging.INFO)
    parser.add_argument(
        '-vv',
        '--very-verbose',
        dest="loglevel",
        help="set loglevel to DEBUG",
        action='store_const',
        const=logging.DEBUG)
    return parser.parse_args(args)


def setup_logging(loglevel):
    """Setup basic logging

    Args:
      loglevel (int): minimum loglevel for emitting messages
    """
    logformat = "[%(asctime)s] %(levelname)s:%(name)s:%(message)s"
    logging.basicConfig(level=loglevel, stream=sys.stdout,
                        format=logformat, datefmt="%Y-%m-%d %H:%M:%S")


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


def main(args):
    """Main entry point allowing external calls

    Args:
        args ([str]): command line parameter list

    Returns:
        int: Status code - 0 = success, non-zero = failure
    """

    # Overall test suite status: zero indicates success and non-zero indicates failure
    # Initiallize to 0, set to 1 if any test or subtest fails, set to 2 if no tests can be run
    iok = 0

    args = parse_args(args)

    setup_logging(args.loglevel)
    _logger.info("Starting rtest2")

    # Note: dot_exe, tfn, and layout should be specifiable from the command line
    tap_output = []
    pretest_id = 0

    pretest_id += 1
    if exists(args.dot_exe):
        dot_exe = abspath(args.dot_exe)
        tap_output.append("ok {0:d} - Dot executable # Found {1:s}\n".format(pretest_id, dot_exe))
    else:
        tap_output.append("not ok {0:d} - Dot executable # Cannot find {1:s}\n".format(pretest_id, args.dot_exe))
        iok = 2

    pretest_id += 1
    if exists(args.test_json):
        tfn = abspath(args.test_json)
        tap_output.append("ok {0:d} - Test definitions # Found {1:s}\n".format(pretest_id, tfn))
    else:
        tap_output.append("not ok {0:d} - Test definitions # Cannot find {1:s}\n".format(pretest_id, args.test_json))
        iok = 2

    pretest_id += 1
    graph_dir = abspath(args.graph_dir)
    if not exists(graph_dir):
        mkdir(graph_dir)
        _logger.info("Created input directory {0:s}".format(graph_dir))
    if exists(graph_dir):
        tap_output.append("ok {0:d} - Input directory # Found {1:s}\n".format(pretest_id, graph_dir))
    else:
        tap_output.append("not ok {0:d} - Input directory # Cannot create {1:s}\n".format(pretest_id, args.graph_dir))
        iok = 2

    pretest_id += 1
    results_dir = abspath(args.results_dir)
    if not exists(results_dir):
        mkdir(results_dir)
        _logger.info("Created results directory {0:s}".format(results_dir))
    if exists(results_dir):
        tap_output.append("ok {0:d} - Results directory # Found {1:s}\n".format(pretest_id, results_dir))
    else:
        tap_output.append("not ok {0:d} - Results directory # Cannot create {1:s}\n".format(pretest_id, args.results_dir))
        iok = 2

    pretest_id += 1
    if exists(args.reference_dir):
        reference_dir = abspath(args.reference_dir)
        tap_output.append("ok {0:d} - Reference directory # Found {1:s}\n".format(pretest_id, reference_dir))
    else:
        tap_output.append("not ok {0:d} - Reference directory # Cannot find {1:s}\n".format(pretest_id, args.reference_dir))
        iok = 2

    pretest_id += 1
    layout = args.layout.strip().lower()
    if len(layout) > 0:
        tap_output.append("ok {0:d} - Layout engine # Setting to {1:s}\n".format(pretest_id, layout))
    else:
        tap_output.append("not ok {0:d} - Input directory # Cannot find {1:s}\n".format(pretest_id, args.layout))
        iok = 2

    # Remove stale TAP file
    tap_fn = get_tap_filename(results_dir, 0)
    if exists(tap_fn):
        remove(tap_fn)

    add_tap_plan(tap_output)
    if args.write_stdout:
        sys.stdout.writelines(tap_output)
    if args.write_tapfiles:
        write_tap(results_dir, 0, tap_output)

    # Quit if pretests fail
    if iok != 0:
        _logger.info("Prematurely ending rtest2")
        return iok

    # Compatibility flag: set to False for original test.tcl behavior
    use_common_input = True

    # Import test roster from JSON input
    with open(tfn, 'r') as ifh:
        test_group = json.load(ifh, object_pairs_hook=OrderedDict)

    # Generate and run tests
    test_id = 0
    for test_label, test_item in test_group.items():
        test_id += 1

        # Wipe stale TAP output
        tap_output = []

        tap_fn = get_tap_filename(results_dir, test_id)
        if exists(tap_fn):
            remove(tap_fn)

        # TAP output: Print range of subtests
        # nsubtests = len(test_item["format"])
        # tap_output = ["1..{0:d}".format(nsubtests)]

        _logger.info("Test {0:03d}:".format(test_id))
        _logger.info("  Label: {0:s}".format(test_label))
        _logger.info("  Code:  {0:s}".format(test_item["code"]))
        if len(test_item["format"]) == 1:
            _logger.info("  Subtest:")
        else:
            _logger.info("  Subtests:")

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
            _logger.info("    {0:s}: {1:s}".format(tag, " ".join(cmd_args)))

            # Run the test
            pstatus = subprocess.run(cmd_args)

            if (pstatus.returncode != 0):
                _logger.info("Subtest {0:s} errored out with status code {1:d}:"
                    .format(tag, pstatus.returncode))
            else:
                _logger.info("Subtest {0:s} completed successfully"
                    .format(tag))

            if (pstatus.stdout):
                _logger.info("stdout: {0:s}".format(pstatus.stdout))

            if (pstatus.stderr):
                _logger.info("stderr: {0:s}".format(pstatus.stderr))

            # Analyze results
            if exists(results_fn) and exists(reference_fn):
                _logger.info("Found reference file {0:s} corresponding to results file {1:s}"
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
            tap_output.append("{0:s} {1:d} - {2:s}{3:s}\n"
                .format(subtest_status, subtest_id, subtest_label, subtest_comment))

        add_tap_plan(tap_output)
        if args.write_stdout:
            sys.stdout.writelines(tap_output)
        if args.write_tapfiles:
            write_tap(results_dir, test_id, tap_output)

    _logger.info("Ending rtest2")
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
