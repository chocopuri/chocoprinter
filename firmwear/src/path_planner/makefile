CXX := g++
CXXFLAGS := -Wall -std=c++17 -I ../
OBJ := mock_tester.o path_planner.o
TARGET := tester.exe

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	@rm -f $(TARGET) *.o

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
