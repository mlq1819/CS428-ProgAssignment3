S=program
CFLAGS= -g -Wall

all: $(S)

$(S): $(S).cpp
	g++ $(CFLAGS) -o $(S) $(S).cpp
	
clean:
	rm -rf $(S)