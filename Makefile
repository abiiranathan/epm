CXX_FLAGS=-std=c++17 -Wall -Wextra -Werror -pedantic -O3 -Wno-unused-value -static
CXX=g++
LIBS=-lssl -lcrypto
SRCS=main.cpp encryption.cpp password.cpp epass.cpp
TARGET=epass


all: $(SRCS)
	$(CXX) $(CXX_FLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f main