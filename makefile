CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -fPIC
LDFLAGS = -shared

# Цель по умолчанию – сборка приложения
all: logger_app

# Сборка динамической библиотеки
logger_lib.so: Logger.o
	$(CXX) $(LDFLAGS) -o $@ $^

Logger.o: Logger.cpp Logger.h
	$(CXX) $(CXXFLAGS) -c Logger.cpp

# Сборка тестового приложения
tests: tests.o logger_lib.so
	$(CXX) $(CXXFLAGS) -Wl,-rpath,. -o tests tests.o -L. -l:logger_lib.so

tests.o: tests.cpp Logger.h
	$(CXX) $(CXXFLAGS) -c tests.cpp

# Сборка консольного приложения
logger_app: main.o logger_lib.so
	$(CXX) $(CXXFLAGS) -Wl,-rpath,. -o logger_app main.o -L. -l:logger_lib.so

main.o: main.cpp Logger.h
	$(CXX) $(CXXFLAGS) -c main.cpp

# Цель очистки
clean:
	rm -f *.o logger_lib.so logger_app tests
