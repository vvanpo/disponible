DAEMON=disponible
CLIENT=dsp_client

CFLAGS=-std=c11 -Wall -Wno-parentheses
DEBUG=-g
NODEBUG=-DNDEBUG

all: $(DAEMON) $(CLIENT)

$(DAEMON):
	$(MAKE) -C node

$(CLIENT):
	$(MAKE) -C client

clean:
	rm -f $(DAEMON) $(CLIENT)
