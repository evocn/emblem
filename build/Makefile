
SRC = $(wildcard ../src/*.cpp ../ext/imgui/*.cpp ../ext/imgui/misc/cpp/*.cpp ../ext/imgui/backends/*.cpp)
OBJ = ${SRC:.cpp=.o}

LDFLAGS = -L /opt/homebrew/lib -lSDL2_image -lSDL2 -lSDL2_ttf
INCFLAGS = -I../src/ -I../ext/ -I../ext/imgui/ -I../ext/imgui/backends -I/opt/homebrew/include/SDL2
FLAGS = -std=c++14 -Wno-deprecated -glldb -O0 # -Wall 
OUT = em

CC = clang++

# For debugging
print-%  : ; @echo $* = $($*)

$(OUT): $(OBJ)
	@$(CC) $(FLAGS) $(INCFLAGS) $(LDFLAGS) $^ -o $@
	@printf "\e[33mLinking\e[90m %s\e[0m\n" $@
	@printf "\e[34mDone!\e[0m\n"

%.o: %.cpp
	@$(CC) $(FLAGS) $(INCFLAGS) $(WINDOWS) -c $< -o $@
	@printf "\e[36mCompile\e[90m %s\e[0m\n" $@

../src/emblem.o: ../src/*.h

clean:
	@rm -f $(OUT) $(OBJ)
	@printf "\e[34mAll clear!\e[0m\n"
