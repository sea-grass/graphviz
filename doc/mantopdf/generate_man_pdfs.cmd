@REM System overview
pandoc -f man -t pdf --pdf-engine=xelatex -o graphviz.7.pdf ..\..\graphviz.7

@REM Applications
pandoc -f man -t pdf --pdf-engine=xelatex -o dot.1.pdf ..\..\cmd\dot\dot.1
pandoc -f man -t pdf --pdf-engine=xelatex -o osage.1.pdf ..\..\cmd\dot\osage.1
pandoc -f man -t pdf --pdf-engine=xelatex -o patchwork.1.pdf ..\..\cmd\dot\patchwork.1
pandoc -f man -t pdf --pdf-engine=xelatex -o dotty.1.pdf ..\..\cmd\dotty\dotty.1
pandoc -f man -t pdf --pdf-engine=xelatex -o edgepaint.1.pdf ..\..\cmd\edgepaint\edgepaint.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvedit.1.pdf ..\..\cmd\gvedit\gvedit.1
pandoc -f man -t pdf --pdf-engine=xelatex -o cluster.1.pdf ..\..\cmd\gvmap\cluster.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvmap.1.pdf ..\..\cmd\gvmap\gvmap.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvmap.sh.1.pdf ..\..\cmd\gvmap\gvmap.sh.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvpr.1.pdf ..\..\cmd\gvpr\gvpr.1
pandoc -f man -t pdf --pdf-engine=xelatex -o lefty.1.pdf ..\..\cmd\lefty\lefty.1
pandoc -f man -t pdf --pdf-engine=xelatex -o lneato.1.pdf ..\..\cmd\lneato\lneato.1
pandoc -f man -t pdf --pdf-engine=xelatex -o mingle.1.pdf ..\..\cmd\mingle\mingle.1
pandoc -f man -t pdf --pdf-engine=xelatex -o smyrna.1.pdf ..\..\cmd\smyrna\smyrna.1
pandoc -f man -t pdf --pdf-engine=xelatex -o acyclic.1.pdf ..\..\cmd\tools\acyclic.1
pandoc -f man -t pdf --pdf-engine=xelatex -o bcomps.1.pdf ..\..\cmd\tools\bcomps.1
pandoc -f man -t pdf --pdf-engine=xelatex -o ccomps.1.pdf ..\..\cmd\tools\ccomps.1
pandoc -f man -t pdf --pdf-engine=xelatex -o dijkstra.1.pdf ..\..\cmd\tools\dijkstra.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gc.1.pdf ..\..\cmd\tools\gc.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gml2gv.1.pdf ..\..\cmd\tools\gml2gv.1
pandoc -f man -t pdf --pdf-engine=xelatex -o graphml2gv.1.pdf ..\..\cmd\tools\graphml2gv.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvcolor.1.pdf ..\..\cmd\tools\gvcolor.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvgen.1.pdf ..\..\cmd\tools\gvgen.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gvpack.1.pdf ..\..\cmd\tools\gvpack.1
pandoc -f man -t pdf --pdf-engine=xelatex -o gxl2gv.1.pdf ..\..\cmd\tools\gxl2gv.1
pandoc -f man -t pdf --pdf-engine=xelatex -o mm2gv.1.pdf ..\..\cmd\tools\mm2gv.1
pandoc -f man -t pdf --pdf-engine=xelatex -o nop.1.pdf ..\..\cmd\tools\nop.1
pandoc -f man -t pdf --pdf-engine=xelatex -o sccmap.1.pdf ..\..\cmd\tools\sccmap.1
pandoc -f man -t pdf --pdf-engine=xelatex -o tred.1.pdf ..\..\cmd\tools\tred.1
pandoc -f man -t pdf --pdf-engine=xelatex -o unflatten.1.pdf ..\..\cmd\tools\unflatten.1
pandoc -f man -t pdf --pdf-engine=xelatex -o diffimg.1.pdf ..\..\contrib\diffimg\diffimg.1
pandoc -f man -t pdf --pdf-engine=xelatex -o prune.1.pdf ..\..\contrib\prune\prune.1
pandoc -f man -t pdf --pdf-engine=xelatex -o vimdot.1.pdf ..\..\plugin\xlib\vimdot.1
pandoc -f man -t pdf --pdf-engine=xelatex -o inkpot.1.pdf ..\..\lib\inkpot\inkpot.1

@REM Libraries
pandoc -f man -t pdf --pdf-engine=xelatex -o agraph.3.pdf ..\..\lib\agraph\agraph.3
pandoc -f man -t pdf --pdf-engine=xelatex -o cdt.3.pdf ..\..\lib\cdt\cdt.3
pandoc -f man -t pdf --pdf-engine=xelatex -o cgraph.3.pdf ..\..\lib\cgraph\cgraph.3
pandoc -f man -t pdf --pdf-engine=xelatex -o lab_gamut.3.pdf ..\..\lib\edgepaint\lab_gamut.3
pandoc -f man -t pdf --pdf-engine=xelatex -o expr.3.pdf ..\..\lib\expr\expr.3
pandoc -f man -t pdf --pdf-engine=xelatex -o gvc.3.pdf ..\..\lib\gvc\gvc.3
pandoc -f man -t pdf --pdf-engine=xelatex -o gvpr.3.pdf ..\..\lib\gvpr\gvpr.3
pandoc -f man -t pdf --pdf-engine=xelatex -o inkpot.3.pdf ..\..\lib\inkpot\inkpot.3
pandoc -f man -t pdf --pdf-engine=xelatex -o pack.3.pdf ..\..\lib\pack\pack.3
pandoc -f man -t pdf --pdf-engine=xelatex -o pathplan.3.pdf ..\..\lib\pathplan\pathplan.3
pandoc -f man -t pdf --pdf-engine=xelatex -o xdot.3.pdf ..\..\lib\xdot\xdot.3
