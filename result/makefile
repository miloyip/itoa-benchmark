%.html: %.csv
	php template.php $< > $@

CSVFILES = $(basename $(wildcard *.csv))
all: $(addsuffix .html, $(CSVFILES))
