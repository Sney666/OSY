CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer -std=c99
LDFLAGS = -fsanitize=address

all: libcharutil.a upper_lib/libtransform.so lower_lib/libtransform.so swap_lib/libtransform.so strtransform genstr small_lib/libstrcheck.so big_lib/libstrcheck.so strcheck

libcharutil.a: libcharutil.c
	$(CC) $(CFLAGS) -fPIC -c libcharutil.c -o libcharutil.o
	ar rcs libcharutil.a libcharutil.o

upper_lib/libtransform.so: upper_lib/upper_lib.c libcharutil.a
	$(CC) $(CFLAGS) -fPIC -shared upper_lib/upper_lib.c -L. -lcharutil -o upper_lib/libtransform.so

lower_lib/libtransform.so: lower_lib/lower_lib.c libcharutil.a
	$(CC) $(CFLAGS) -fPIC -shared lower_lib/lower_lib.c -L. -lcharutil -o lower_lib/libtransform.so

swap_lib/libtransform.so: swap_lib/swap_lib.c libcharutil.a
	$(CC) $(CFLAGS) -fPIC -shared swap_lib/swap_lib.c -L. -lcharutil -o swap_lib/libtransform.so

strtransform: strtransform.c libcharutil.a upper_lib/libtransform.so
	$(CC) $(CFLAGS) strtransform.c -L. -lcharutil -Lupper_lib -ltransform -o strtransform $(LDFLAGS)

libgenstr.a: genstr_lib.c
	$(CC) $(CFLAGS) -c genstr_lib.c -o genstr_lib.o
	ar rcs libgenstr.a genstr_lib.o

genstr: genstr.c libgenstr.a
	$(CC) $(CFLAGS) genstr.c -L. -lgenstr -o genstr $(LDFLAGS)

small_lib/libstrcheck.so: small_lib/strcheck_lib_small.c
	$(CC) $(CFLAGS) -fPIC -shared small_lib/strcheck_lib_small.c -o small_lib/libstrcheck.so

big_lib/libstrcheck.so: big_lib/strcheck_lib_big.c
	$(CC) $(CFLAGS) -fPIC -shared big_lib/strcheck_lib_big.c -o big_lib/libstrcheck.so

strcheck: strcheck.c small_lib/libstrcheck.so
	$(CC) $(CFLAGS) strcheck.c -Lsmall_lib -lstrcheck -o strcheck $(LDFLAGS)

test: all
	@printf "Hello World\nAbC xYz\n" > test_in.txt
	@printf "HELLO WORLD\nABC XYZ\n" > test_upper.txt
	@printf "hello world\nabc xyz\n" > test_lower.txt
	@printf "hELLO wORLD\naBc XyZ\n" > test_swap.txt
	@LD_LIBRARY_PATH=upper_lib ./strtransform < test_in.txt > out_upper.txt
	@cmp -s out_upper.txt test_upper.txt && echo "upper: PASS" || echo "upper: FAIL"
	@LD_LIBRARY_PATH=lower_lib ./strtransform < test_in.txt > out_lower.txt
	@cmp -s out_lower.txt test_lower.txt && echo "lower: PASS" || echo "lower: FAIL"
	@LD_LIBRARY_PATH=swap_lib ./strtransform < test_in.txt > out_swap.txt
	@cmp -s out_swap.txt test_swap.txt && echo "swap: PASS" || echo "swap: FAIL"
	@rm -f test_*.txt out_*.txt

clean:
	rm -f *.o *.a strtransform genstr strcheck upper_lib/*.so lower_lib/*.so swap_lib/*.so small_lib/*.so big_lib/*.so