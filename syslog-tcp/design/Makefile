html:
	docbook2html -u lrsp.docbook
	-tidy -q -asxhtml lrsp.html > lrsp.xhtml 2>/dev/null

txt:
	docbook2txt -u lrsp.docbook
all: html txt
clean:
	rm -f lrsp.txt
	rm -f lrsp.html
	rm -f lrsp.xhtml
