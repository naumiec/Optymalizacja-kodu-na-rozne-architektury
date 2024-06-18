CC         := gcc
LINKER     := $(CC)
CFLAGS     := -Wall -msse3 -march=native
CFLAGS_AVX := -Wall -mavx -march=native
CFLAGS_AVX512 := -Wall -mavx512f -march=native
FLAGS_PAPI := $(shell pkg-config --cflags --libs papi) -lm
LDFLAGS    := -lm

all:
	make run_all
	make plot_all

run:
	$(CC) $(CFLAGS) $(VERSION).c -o $(VERSION).out $(LDFLAGS)
	./$(VERSION).out $(SIZE) >> output_$(VERSION).m

run_avx: 
	$(CC) $(CFLAGS_AVX) $(VERSION).c -o $(VERSION).out $(LDFLAGS)
	./$(VERSION).out $(SIZE) >> output_$(VERSION).m

run_avx512:
	$(CC) $(CFLAGS_AVX512) $(VERSION).c -o $(VERSION).out $(LDFLAGS)
	./$(VERSION).out $(SIZE) >> output_$(VERSION).m

run_o2:
	$(CC) $(CFLAGS) -O2 $(VERSION).c -o $(VERSION).out $(LDFLAGS)
	./$(VERSION).out $(SIZE) >> output_o2_$(VERSION).m

run_avx_o2: 
	$(CC) $(CFLAGS_AVX) -O2 $(VERSION).c -o $(VERSION).out $(LDFLAGS)
	./$(VERSION).out $(SIZE) >> output_o2_$(VERSION).m

run_avx512_o2:
	$(CC) $(CFLAGS_AVX512) -O2 $(VERSION).c -o $(VERSION).out $(LDFLAGS)
	./$(VERSION).out $(SIZE) >> output_o2_$(VERSION).m

run_all:
	for v in $(shell seq 1 7) ; do \
		echo "version = ge$$v" > output_ge$$v.m; \
		for s in $$(seq 1000 500 3000) ; do \
			make run VERSION="ge$$v" SIZE=$$s; \
		done ; \
	done
	echo "version = ge8" > output_ge8.m;
	for s in $$(seq 1000 500 3000) ; do \
		make run_avx512 VERSION="ge8" SIZE=$$s; \
	done ; \

run_all_o2:
	for v in $(shell seq 1 7) ; do \
		echo "version = ge$$v" > output_o2_ge$$v.m; \
		for s in $$(seq 1000 500 3000) ; do \
			make run_o2 VERSION="ge$$v" SIZE=$$s; \
		done ; \
	done
	echo "version = ge8" > output_o2_ge8.m;
	for s in $$(seq 1000 500 3000) ; do \
		make run_avx512_o2 VERSION="ge8" SIZE=$$s; \
	done ; \

run_papi:
	$(CC) $(CFLAGS) $(FLAGS_PAPI) $(VERSION).c -o $(VERSION).out
	./$(VERSION).out $(SIZE) >> output_$(VERSION).m
	
run_papi_avx:
	$(CC) $(CFLAGS_AVX) $(FLAGS_PAPI) $(VERSION).c -o $(VERSION).out
	./$(VERSION).out $(SIZE) >> output_$(VERSION).m

run_papi_avx512:
	$(CC) $(CFLAGS_AVX512) $(FLAGS_PAPI) $(VERSION).c -o $(VERSION).out
	./$(VERSION).out $(SIZE) >> output_$(VERSION).m

run_all_papi:
	for v in $(shell seq 1 7) ; do \
		echo "version = ge$$v" > output_ge$$v.m; \
		for s in $$(seq 1000 500 3000) ; do \
			make run_papi VERSION="ge$$v" SIZE=$$s; \
		done ; \
	done
	echo "version = ge8" > output_ge8.m;
	for s in $$(seq 1000 500 3000) ; do \
		make run_papi_avx512 VERSION="ge8" SIZE=$$s; \
	done ; \

run_papi_o2:
	$(CC) -O2 $(CFLAGS) $(FLAGS_PAPI) $(VERSION).c -o $(VERSION).out
	./$(VERSION).out $(SIZE) >> output_o2_$(VERSION).m
	
run_papi_avx_o2:
	$(CC) -O2 $(CFLAGS_AVX) $(FLAGS_PAPI) $(VERSION).c -o $(VERSION).out
	./$(VERSION).out $(SIZE) >> output_o2_$(VERSION).m

run_papi_avx512_o2:
	$(CC) -O2 $(CFLAGS_AVX512) $(FLAGS_PAPI) $(VERSION).c -o $(VERSION).out
	./$(VERSION).out $(SIZE) >> output_o2_$(VERSION).m

run_all_papi_o2:
	for v in $(shell seq 1 7) ; do \
		echo "version = ge$$v" > output_o2_ge$$v.m; \
		for s in $$(seq 1000 500 3000) ; do \
			make run_papi_o2 VERSION="ge$$v" SIZE=$$s; \
		done ; \
	done
	echo "version = ge8" > output_o2_ge8.m;
	for s in $$(seq 1000 500 3000) ; do \
		make run_papi_avx512_o2 VERSION="ge8" SIZE=$$s; \
	done ; \

plot_all:
	./output_to_csv.sh
	./plot_all.py

do_all:
	make run_all
	make plot_all

cleanall:
	rm -f output*.m *.out
