all: main.o tokenizer.o recursive_descent_parser.o symbol_table.o abstract_syntax_tree.o interpreter.o debug clean

debug:
	g++ main.o tokenizer.o recursive_descent_parser.o symbol_table.o abstract_syntax_tree.o interpreter.o -o ffi

main.o:
	g++ -c main.cpp

tokenizer.o:
	g++ -c tokenizer.cpp

recursive_descent_parser.o:
	g++ -c recursive_descent_parser.cpp

symbol_table.o:
	g++ -c symbol_table.cpp

abstract_syntax_tree.o:
	g++ -c abstract_syntax_tree.cpp

interpreter.o:
	g++ -c interpreter.cpp

clean:
	rm interpreter.o

clean-all:
	rm main.o
	rm tokenizer.o
	rm recursive_descent_parser.o
	rm symbol_table.o
	rm abstract_syntax_tree.o
	rm interpreter.o