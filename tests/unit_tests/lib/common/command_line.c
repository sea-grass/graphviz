#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include <stdbool.h>

#include "builddate.h"
#include "config.h"
#include "gvc.h"

static void redirect_all_std(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

lt_symlist_t lt_preloaded_symbols[] = { { 0, 0 } };
extern int GvExitOnUsage;

/**
 * Exit and output tests for `dot -V`
 */
Test(command_line, dash_V_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-V"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_V_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-V"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[100];
	snprintf(expected_stderr, sizeof(expected_stderr),
	         "dot - graphviz version %s (%s)\n", PACKAGE_VERSION, BUILDDATE);

	cr_assert_stderr_eq_str(expected_stderr);
}

/**
 * Exit and output tests for `dot -Vrandom`
 */
Test(command_line, dash_Vrandom_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-Vrandom"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_Vrandom_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-Vrandom"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[100];
	snprintf(expected_stderr, sizeof(expected_stderr),
	         "dot - graphviz version %s (%s)\n", PACKAGE_VERSION, BUILDDATE);

	cr_assert_stderr_eq_str(expected_stderr);
}

/**
 * Exit and output tests for `dot -V?`
 */
Test(command_line, dash_V_questionmark_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-V?"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_V_questionmark_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-V?"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[100];
	snprintf(expected_stderr, sizeof(expected_stderr),
	         "dot - graphviz version %s (%s)\n", PACKAGE_VERSION, BUILDDATE);

	cr_assert_stderr_eq_str(expected_stderr);
}
