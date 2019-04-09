CXX = g++
FILES = rlg327.cpp dungeon.cpp priority_queue.cpp character_utils.cpp heap.cpp dice.cpp character.cpp parser.cpp item.cpp io.cpp item_utils.cpp

RM = rm -rf

CXXFLAGS = -Wall -ggdb3 #-Werror
LDFLAGS = -lncurses
ECHO = echo

rlg327: $(FILES)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FILES)

clean:
	@$(RM) rlg327 *.o *.d *.dSYM *~

cleansubmit:
	@$(RM) .vscode .git

submit: clean cleansubmit
rebuild: clean rlg327
