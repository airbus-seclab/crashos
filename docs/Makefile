all: rapport doxygen

files := rapport.tex

rapport: $(files)
	@pdflatex rapport.tex
	@pdflatex rapport.tex
	@pdflatex rapport.tex

clean:
	@rm -f rapport.pdf *.aux *.log *.blg *.toc *.bbl *.tex~ *.out
	@rm -f -r doxygen

doxygen:
	@mkdir -p doxygen
	@doxygen ../tools/Doxyfile