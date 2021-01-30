cd ../src
xelatex -halt-on-error -output-directory ../out Bericht.tex
bibtex ../out/Bericht
xelatex -halt-on-error -output-directory ../out Bericht.tex
xelatex -halt-on-error -output-directory ../out Bericht.tex
cd ../out
xcopy Bericht.pdf .. /q/y
cd ../compile