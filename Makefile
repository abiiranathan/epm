CXXFLAGS=-I./include -std=c++17 -Wall -Wextra -Werror -pedantic -O3 -Wno-unused-value
CXX=g++
LIBS=-lssl -lcrypto -lsodium

SOURCEDIR := ./src
OBJDIR := ./obj

SRCS := $(wildcard $(SOURCEDIR)/*.cpp)
OBJS := $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

TARGET=epm

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean