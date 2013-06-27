CC:= gcc
LD:= gcc
CFLAGS:= -c -Wall -Ilib -pthread
LDFLAGS:= -pthread

LIBS:= lib/io_utils.c lib/sync_utils.c lib/list.c

OBJS:= main.o processor.o $(LIBS:.c=.o)

MAIN_HEADERS:= $(LIBS:.c=.h) lib/project_types.h
PROC_HEADERS:= lib/io_utils.h lib/sync_utils.h lib/project_types.h

all: main.x

main.x: $(OBJS)
	@echo Linking $@
	@$(LD) $(LDFLAGS) -o $@ $^
	
main.o: main.c $(MAIN_HEADERS)
	@echo $@
	@$(CC) $(CFLAGS) $< -o $@
	
processor.o: processor.c $(PROC_HEADERS)
	@echo $@
	@$(CC) $(CFLAGS) $< -o $@
	
lib/io_utils.o: lib/io_utils.c lib/io_utils.h
	@echo $@
	@$(CC) $(CFLAGS) $< -o $@
	
lib/list.o: lib/list.c lib/list.h
	@echo $@
	@$(CC) $(CFLAGS) $< -o $@

lib/sync_utils.o: lib/sync_utils.c lib/sync_utils.h lib/io_utils.h
	@echo $@
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f *.o lib/*.o main.x

.PHONY: all clean
