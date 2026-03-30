CXX = g++

CXXFLAGS = -g -Wall -Wextra -MMD -MP -I./imgui -I./imgui/backends

LIBS = -lglfw -lGL

TARGET = db_engine

USER_SRCS = main.cpp \
            00Database.cpp \
            01Crud.cpp \
            02Query.cpp \
            03Interpreter.cpp \
            04Parser.cpp \
            05Interface.cpp

IMGUI_SRCS = imgui/imgui.cpp \
             imgui/imgui_draw.cpp \
             imgui/imgui_tables.cpp \
             imgui/imgui_widgets.cpp \
             imgui/backends/imgui_impl_glfw.cpp \
             imgui/backends/imgui_impl_opengl3.cpp \
             imgui/misc/cpp/imgui_stdlib.cpp

SRCS = $(USER_SRCS) $(IMGUI_SRCS)

OBJS = $(SRCS:.cpp=.o)

DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking final executable: $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "Build complete! Run with ./$(TARGET)"

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Wiping object files, dependency maps, and binaries..."
	rm -f $(OBJS) $(DEPS) $(TARGET)

.PHONY: all clean

-include $(DEPS)