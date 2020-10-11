
#simulator : bin/simulator
#bin/simulator : src/simulator.cpp
#	g++ src/simulator.cpp -o bin/simulator

simulator: simulator.o cpu.o
	g++ simulator.o cpu.o -o bin/simulator

simulator.o: src/simulator.cpp
	g++ -c src/simulator.cpp

cpu.o: src/cpu.hpp src/cpu.cpp
	g++ -c src/cpu.cpp

#extract.o: src/extract.cpp src/extract.hpp
#	g++ -c src/extract.cpp

#execute.o: src/execute.cpp src/execute.hpp
	#g++ -c src/execute.cpp
