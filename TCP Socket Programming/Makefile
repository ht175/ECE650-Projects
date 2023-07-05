# TARGETS=ringmaster player

# all: $(TARGETS)

# clean:
# 	rm -f $(TARGETS)

# ringmaster: ringmaster.cpp function.h potato.h
# 	g++ -g -o $@ $<

# player: player.cpp function.h potato.h
# 	g++ -g -o $@ $<
all: ringmaster player

ringmaster: ringmaster.cpp function.cpp potato.hpp
	g++ -g -o ringmaster ringmaster.cpp function.cpp 
player: player.cpp function.cpp potato.hpp
	g++ -g -o player player.cpp function.cpp 

.PHONY:
	clean
clean:
	rm -rf *.o ringmaster player
