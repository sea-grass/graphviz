The exising nroff(?) formatted UNIX man pages were manually converted to PDF format using [pandoc](https://pandoc.org/) and [LaTeX](https://www.tug.org/texlive/), specifically `xelatex`. Two scripts are provided to perform bulk conversion: `generate_man_pdfs.cmd` is a Windows command ('batch') file and `generate_man_pdfs.sh` is a virtually identical Bourne shell script. Neither are particularly robust but they do the job for now.

Minimal processing was done to the original files and `pandoc` complains about the presence of Unicode Character 'SIX-PER-EM SPACE' (U+2006), a typographical 'thin space'.

Ideally and eventually the man pages and PDF documents would both be rendered from common documentation source files. At present however `pandoc` and LaTeX provide a cross-platform means of converting the man pages to PDF format for use on systems where `man` is not available.
