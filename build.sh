mkdir _build 2>/dev/null >/dev/null
g++ -o _build/cpp_print_all_ident_vals cpp_print_all_ident_vals.cpp
cp _build/cpp_print_all_ident_vals ~/bin/
