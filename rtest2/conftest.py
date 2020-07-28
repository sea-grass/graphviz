from os.path import abspath, exists, join
import pytest

def pytest_addoption(parser):
    parser.addoption(
        "--rtest2-test-definitions",
        help="set JSON test definition file",
        default=join(abspath("."), "TESTS.json"),
        action="store")
    parser.addoption(
        "--rtest2-dot-executable",
        help="set dot executable",
        default=join(abspath("."), "dot"),
        action="store")
    parser.addoption(
        "--rtest2-input-dir",
        help="set directory to contain generated input files",
        default=join(abspath("."), "test_graphs"),
        action="store")
    parser.addoption(
        "--rtest2-output-dir",
        help="set directory to contain generated results",
        default=join(abspath("."), "test_results"),
        action="store")
    parser.addoption(
        "--rtest2-reference-dir",
        help="set directory containing reference results",
        default=join(abspath("."), "test_reference"),
        action="store")
    parser.addoption(
        "--rtest2-layout",
        help="set layout engine",
        default="dot",
        action="store")


@pytest.fixture
def arg_dot_exe(request):
    return request.config.getoption("--rtest2-dot-executable")

@pytest.fixture
def arg_reference_dir(request):
    return request.config.getoption("--rtest2-reference-dir")

@pytest.fixture
def arg_test_json(request):
    return request.config.getoption("--rtest2-test-definitions")

@pytest.fixture
def arg_layout(request):
    return request.config.getoption("--rtest2-layout")

@pytest.fixture
def arg_graph_dir(request):
    return request.config.getoption("--rtest2-input-dir")

@pytest.fixture
def arg_results_dir(request):
    return request.config.getoption("--rtest2-output-dir")

@pytest.fixture
def arg_write_stdout(request):
    return False

@pytest.fixture
def arg_write_tapfiles(request):
    return True

@pytest.fixture
def arg_use_pytest(request):
    return True