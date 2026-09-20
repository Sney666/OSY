# Překladač a základní parametry
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Když do terminálu napíšeš jen "make", udělá to příkaz "all", 
# který postupně vyvolá tvorbu všeho níže.
all: genstr small_lib/libstrcheck.so big_lib/libstrcheck.so strcheck

# ---------------------------------------------------------
# 1. ČÁST: GENSTR A STATICKÁ KNIHOVNA
# ---------------------------------------------------------

# Pravidlo pro vytvoření statické knihovny libgenstr.a
libgenstr.a: genstr_lib.c
	$(CC) $(CFLAGS) -c genstr_lib.c -o genstr_lib.o
	ar rcs libgenstr.a genstr_lib.o

# Pravidlo pro hlavní program genstr (připojujeme naši statickou knihovnu)
genstr: genstr.c libgenstr.a
	$(CC) $(CFLAGS) genstr.c -L. -lgenstr -o genstr

# ---------------------------------------------------------
# 2. ČÁST: STRCHECK A DYNAMICKÉ KNIHOVNY
# ---------------------------------------------------------

# Vytvoření dynamické knihovny pro malá písmena (.so)
small_lib/libstrcheck.so: small_lib/strcheck_lib_small.c
	$(CC) $(CFLAGS) -fPIC -shared small_lib/strcheck_lib_small.c -o small_lib/libstrcheck.so

# Vytvoření dynamické knihovny pro velká písmena (.so)
big_lib/libstrcheck.so: big_lib/strcheck_lib_big.c
	$(CC) $(CFLAGS) -fPIC -shared big_lib/strcheck_lib_big.c -o big_lib/libstrcheck.so

# Pravidlo pro hlavní program strcheck 
# (Lsmall_lib je tam jen proto, aby to prošlo kompilací, za běhu to přepíše LD_LIBRARY_PATH)
strcheck: strcheck.c small_lib/libstrcheck.so
	$(CC) $(CFLAGS) strcheck.c -Lsmall_lib -lstrcheck -o strcheck

# ---------------------------------------------------------
# ÚKLID
# ---------------------------------------------------------
# Příkaz "make clean" smaže všechny vygenerované soubory
clean:
	rm -f *.o *.a *.so genstr strcheck small_lib/*.so big_lib/*.so