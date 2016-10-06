CFLAGS := -Wall --std=gnu99 -g -Wno-format-security
CURL_LIBS := $(shell curl-config --libs)
CURL_CFLAGS := $(shell curl-config --cflags)

ARCH := $(shell uname)
ifneq ($(ARCH),Darwin)
  LDFLAGS += -lpthread
endif

all: gfclient_metrics

gfclient_metrics: gfclient.o workload.o gfclient_metrics.o steque.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) 

.PHONY: clean

clean:
	mv gfclient.o gfclient.o.tmp
	rm -fr *.o gfclient_metrics
	mv gfclient.o.tmp gfclient.o