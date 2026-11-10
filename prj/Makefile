# ==================== Компилятор и флаги ====================
CXX      := g++
CXXSTD   := -std=c++20
WARN     := -Wall -Wextra -Wpedantic
INC      := -Iinclude

LIBS	 := -lcpr -lcurl

# nlohmann_json — header-only, достаточно include-пути (обычно уже в /usr/include или nix-профиле)
CXXFLAGS := $(CXXSTD) $(WARN) $(INC) $(LIBS)

# ==================== Исходники и цели ====================
SRC      := main.cpp
OBJ      := $(SRC:.cpp=.o)
TARGET   := main.run

# ==================== Правила ====================
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
