CXX = g++
FILES = rlg327.cpp dungeon.cpp priority_queue.cpp path.cpp characters.cpp heap.cpp user_interface.cpp monster_parser.cpp npc.cpp dice.cpp

RM = rm -rf

CXXFLAGS = -Wall -ggdb3
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
