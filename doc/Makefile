
PANDOC_OPT=--from markdown+autolink_bare_uris --pdf-engine=xelatex --template eisvogel.tex --toc --listings --number-sections

all: InternalDistributionGuide.pdf

%.pdf: %.md eisvogel.tex
	pandoc $< -o $@ $(PANDOC_OPT)

clean:
	rm *.pdf
