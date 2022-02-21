"""
runner for Graphviz unit tests
"""

import os
from pathlib import Path
import sys
import pytest

sys.path.append(os.path.dirname(__file__))
from gvtest import run_c #pylint: disable=C0413

@pytest.mark.parametrize("case", (
  "dash_V_exit.c",
  "dash_V_output.c",
  "dash_V_questionmark_exit.c",
  "dash_V_questionmark_output.c",
  "dash_Vrandom_exit.c",
  "dash_Vrandom_output.c",
  "dash_questionmark_V_exit.c",
  "dash_questionmark_V_output.c",
  "dash_randomV_exit.c",
  "dash_randomV_output.c",
))
def test_unit(case: str):
  """
  run a unit test
  """

  # locate the source code for this test
  src = Path(__file__).parent / "unit_tests" / case
  assert src.exists(), "test case not found"

  # compile and run the test case
  stdout, stderr = run_c(src, cflags=["-DDEMAND_LOADING=1"],
                         link=["cgraph", "gvc"])

  # if this was a test of output content, compare the two streams
  if stdout.startswith("expected: "):
    assert stdout[len("expected: "):] == stderr, "expected output mismatch"
