#!/usr/bin/env python3
# Generates shapes.html. Takes path to compiled HTML grammar and a dot file
# showing HTML labels, to include as argv[1] and argv[2], and a shapelist on
# stdin.

import markupsafe
import sys
import templates

N_PER_ROW = 4

shapes = [line.strip() for line in sys.stdin]

# From https://stackoverflow.com/a/312464/171898
def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]

# Use the shapes name in shape list to create the
# contents of an HTML array.

template = templates.env().get_template('shapes.html.j2')
print(template.render(
    html_grammar=markupsafe.Markup(open(sys.argv[1]).read()),
    html1_dot=markupsafe.Markup(open(sys.argv[2]).read()),
    rows=chunks(shapes, N_PER_ROW),
))
