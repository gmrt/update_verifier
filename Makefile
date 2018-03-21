CC := g++
TARGET := verifier
SRCS := $(wildcard *.cpp) $(wildcard libverifier/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
CFLAGS := -Icompat -Iboringssl/include -pthread

LIBCRYPTO_PATH := boringssl-build
ifeq ($(LIBCRYPTO),)
LIBCRYPTO := $(LIBCRYPTO_PATH)/crypto/libcrypto.a
endif

all: $(TARGET)

$(TARGET): $(LIBCRYPTO) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBCRYPTO)

$(LIBCRYPTO):
	mkdir -p $(LIBCRYPTO_PATH) && cd $(LIBCRYPTO_PATH) && cmake -DCMAKE_BUILD_TYPE=Release ../boringssl && $(MAKE)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

clobber: clean
	rm -rf $(LIBCRYPTO_PATH)

.PHONY: all clean clobber
