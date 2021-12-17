find . -name massif* -print -delete; g++ -g main.cpp -o main; valgrind --tool=massif --stacks=yes --time-unit=B ./main; ms_print massif.out.*
