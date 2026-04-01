# Compile all source files and produce the 'main' executable
build:
    g++ main.cpp keys.cpp -o main -lGL -lglut

# Build the project (if needed) and run the executable
run: build
    ./main
