"#!/bin/bash\n"
"TMPNAME=tempA1R2C3W4i5e6r6d7\n"
"CIRCUIT_MACROS_PATH=/home/agostine/.kde/share/apps/cirkuit/circuit_macros\n"
"\n"
"function mkltx()\n"
"{\n"
"cat > $TMPNAME.tex << EOF\n"
"\\documentclass{article}\n"
"\\usepackage{pstricks,pst-eps,boxdims,graphicx,ifpdf,pst-grad}\n"
"\\pagestyle{empty}\n"
"\\thispagestyle{empty}\n"
"\\begin{document}\n"
"\\newbox\\graph\n"
"\\begin{TeXtoEPS}\n"
"\\input $TMPNAME.cct\n"
"\\box\n"
"\\graph\n"
"\\end{TeXtoEPS}\n"
"\\end{document}\n"
"EOF\n"
"return\n"
"}\n"
"\n"
"filpath=`dirname $1`\n"
"fn=$filpath/`basename $1 .m4`\n"
"if [ -e $fn.m4 ]\n"
"then\n"
"m4 $CIRCUIT_MACROS_PATH/pstricks.m4 $CIRCUIT_MACROS_PATH/libcct.m4 $fn.m4 | dpic -p > $TMPNAME.cct\n"
"if [ ! -e $TMPNAME.tex ]\n"
"then mkltx\n"
"fi\n"
"cp $TMPNAME.cct $fn.pst\n"
"latex $TMPNAME.tex -output-format=dvi\n"
"dvips -Ppdf -G0 -E $TMPNAME -o $fn.eps\n"
"convert -density 200 $fn.eps $fn.png\n"
"#dvips -Ppdf -G0 -O 1in,1in -Pcmz -Pamz $TMPNAME -o $fn.ps\n"
"#ps2eps -l -f $fn.ps\n"
"\n"
"#epstopdf $fn.eps\n"
"#epstopdf -d --nogs $fn.eps>$fn.ps\n"
"\n"
"rm -f $TMPNAME.*\n"
"else\n"
"echo "$fn.m4 does not exist!"\n"
"fi\n"
