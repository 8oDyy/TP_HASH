CC      = gcc
CFLAGS  = -O2 -Wall -std=c11
LDLIBS  = -lm

all: rainbow_create rainbow_attack hash_many

rainbow_create: rainbow_create.c common.h hash.h
	$(CC) $(CFLAGS) -o $@ rainbow_create.c $(LDLIBS)

rainbow_attack: rainbow_attack.c common.h hash.h
	$(CC) $(CFLAGS) -o $@ rainbow_attack.c $(LDLIBS)

hash_many: hash_many.c common.h hash.h
	$(CC) $(CFLAGS) -o $@ hash_many.c $(LDLIBS)

clean:
	rm -f rainbow_create rainbow_attack hash_many
