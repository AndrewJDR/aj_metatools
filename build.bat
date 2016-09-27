REM This is untested
@echo off

SET OPTS=/W4 /wd4310 /wd4100 /wd4201 /wd4505 /wd4996 /wd4127 /wd4510 /wd4512 /wd4610 /wd4390 /WX
SET OPTS=/GR- /EHa- /nologo /FC

cl %OPTS% cpp_print_all_ident_vals.cpp /Zi /Fecpp_print_all_ident_vals
