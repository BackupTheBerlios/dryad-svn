DOC2HTML= docbook2html
DOC2TXT= docbook2txt
DOC2PDF= docbook2pdf
BOOK= dryad-manual
TIDY= tidy

all: html txt pdf

html: $(BOOK).html
	-$(TIDY) -qc -wrap 80  -asxhtml $(BOOK).html > $(BOOK).xhtml
	rm $(BOOK).html
	mv $(BOOK).xhtml $(BOOK).html

txt: $(BOOK).txt

pdf: $(BOOK).pdf

clean:
	rm -f $(BOOK).xhtml
	rm -f $(BOOK).html
	rm -f $(BOOK).pdf
	rm -f $(BOOK).txt

$(BOOK).html: $(BOOK).docbook

%.html : %.docbook
	$(DOC2HTML) -u $<

%.txt : %.docbook
	$(DOC2TXT) $<

%.pdf : %.docbook
	$(DOC2PDF) $<


