## compile
make

## usage:
提示:
./logger


## Monitored file access activities
The list of monitored library calls is shown below. It covers several functions we have introduced in the class.

chmod chown close creat fclose fopen fread fwrite open read remove rename tmpfile write
Output
You have to dump the library calls as well as the corresponding parameters and the return value. We have several special rules for printing out function arguments and return values:

If a passed argument is a filename string, print the real absolute path of the file by using realpath(3). If realpath(3) cannot resolve the filename string, simply print out the string untouched.
If a passed argument is a descriptor or a FILE * pointer, print the absolute path of the corresponding file. The filename for a corresponding descriptor can be found in /proc/{pid}/fd directory.
If a passed argument is a mode or a flag, print out the value in octal.
If a passed argument is an integer, simply print out the value in decimal.
If a passed argument is a regular character buffer, print it out up to 32 bytes. Check each output character using isprint(3) function and output a dot '.' if a character is not printable.
If a return value is an integer, simply print out the value in decimal.
If a return value is a pointer, print out it using %p format conversion specifier.
Output strings should be quoted with double-quotes.

## Hints
When implementing your homework, you may inspect symbols used by an executable. We have mentioned that you are not able to see any symbol if the symbols were stripped using strip command. However, you may consider working with readelf command. For example, we can check the symbols that are unknown to the binary:

$ nm /usr/bin/wget
nm: /usr/bin/wget: no symbols
$ readelf --syms /usr/bin/wget | grep open
    72: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND freopen64@GLIBC_2.2.5 (2)
    73: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND iconv_open@GLIBC_2.2.5 (2)
   103: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND gzdopen
   107: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND fdopen@GLIBC_2.2.5 (2)
   119: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND open64@GLIBC_2.2.5 (2)
   201: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND fopen64@GLIBC_2.2.5 (2)
Alternatively, you may consider using nm -D to read symbols. Basically, we have two different symbol tables. One is the regular symbol table, and the other is the dynamic symbol table. The one removed by strip is the regular symbol table. So you will need to work with nm -D or readelf --syms to read the dynamic symbol table.
