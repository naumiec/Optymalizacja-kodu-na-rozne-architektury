CC = gcc
CFLAGS = -O2 -march=native -mavx -mfma

SRC = $(wildcard ge*.c)
EXE = $(SRC:.c=)

all: $(EXE)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

run_all: all
	for prog in $(EXE); do \
		echo ">>>>> Running $$prog"; \
		./$$prog; \
	done

plot: 
	python3 plot_all.py

clean:
	rm -f $(EXE) *.csv output*.m

.PHONY: all run_all plot clean
