#
# Compiler flags
#
CCXX   = g++
CC     = gcc
CXXFLAGS = -Wall -Werror -Wextra -std=c++20
CFLAGS   = -Wall -Werror -Wextra

#
# Project files
#
CPPSRCS = $(wildcard *.cpp)
CSRCS = $(wildcard *.c)
OBJS = $(CPPSRCS:.cpp=.o) $(CSRCS:.c=.o)
EXE1 = test1
EXE2 = test2

#
# Debug build settings
#
DBGDIR = debug
DBGEXE1 = $(DBGDIR)/$(EXE1)
DBGEXE2 = $(DBGDIR)/$(EXE2)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGDEPS = $(DBGOBJS:%.o=%.d)
DBGFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXE1 = $(RELDIR)/$(EXE1)
RELEXE2 = $(RELDIR)/$(EXE2)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELDEPS = $(RELOBJS:%.o=%.d)
RELFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release remake

# Default build
all: prep release

#
# Debug rules
#
debug: make_dbgdir $(DBGEXE1) $(DBGEXE2)

$(DBGEXE1): $(DBGEXE1).o
		$(CCXX) -o $(DBGEXE1) $^

$(DBGEXE2): $(DBGEXE2).o
		$(CCXX) -o $(DBGEXE2) $^

-include $(DBGDEPS)

$(DBGDIR)/%.o: %.cpp
		$(CCXX) -c $(CXXFLAGS) $(DBGFLAGS) -MMD -o $@ $<

$(DBGDIR)/%.o: %.c
		$(CCXX) -c $(CXXFLAGS) $(DBGFLAGS) -MMD -o $@ $<

#
# Release rules
#
release: make_reldir $(RELEXE1) $(RELEXE2)

$(RELEXE1): $(RELEXE1).o
		$(CCXX) -o $(RELEXE1) $^

$(RELEXE2): $(RELEXE2).o
		$(CCXX) -o $(RELEXE2) $^

-include $(RELDEPS)

$(RELDIR)/%.o: %.cpp
		$(CCXX) -c $(CXXFLAGS) $(RELFLAGS) -MMD -o $@ $<

$(RELDIR)/%.o: %.c
		$(CC) -c $(CFLAGS) $(RELFLAGS) -MMD -o $@ $<

#
# Other rules
#
make_dbgdir:
		@mkdir -p $(DBGDIR)

make_reldir:
		@mkdir -p $(RELDIR)

remake: clean all

clean:
		@rm -r -f $(RELDIR) $(DBGDIR)
