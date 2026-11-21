# Program Executable Rule
# https://stackoverflow.com/questions/39522115/g-cant-find-ncurses-h-despite-it-being-installed
mission_file_builder.out:
	g++ mission_file_builder.cpp -lncursesw -o mission_file_builder.out -I/usr/include/ncursesw

# Run Program
run:
	./mission_file_builder.out

# Remove .out file
clean:
	rm -rf mission_file_builder.out
