CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET = task_scheduler

.PHONY: all run cycle clean

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

run: $(TARGET)
	./$(TARGET) tasks.txt

cycle: $(TARGET)
	-./$(TARGET) cycle_tasks.txt

clean:
	rm -f $(TARGET)
