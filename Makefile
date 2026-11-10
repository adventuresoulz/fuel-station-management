CC      := gcc
CFLAGS  := -std=c99 -Wall -Wextra -Werror -pedantic -g -O0
LDFLAGS :=
TARGET  := fuel_station
SRC     := fuel_station.c


.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
