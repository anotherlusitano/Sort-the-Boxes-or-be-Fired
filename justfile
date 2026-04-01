# Build the project and produce the 'main' executable
build:
    g++ main.cpp -o main -lGL -lglut

# Build the project (if needed) and run it
run: build
    ./main
