start:
	clang++ -std=c++17 src/main.cpp -o TLLang.out

test:
	clang++ -std=c++17 tests/output.cpp -o tests/text.out

clean:
	rm *.out