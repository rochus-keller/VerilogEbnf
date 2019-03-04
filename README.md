NOTE: VerilogEbnf was replaced by EbnfStudio, see https://github.com/rochus-keller/EbnfStudio.

This project includes an LL(1) Verilog 2005 EBNF syntax as well as the tool which helped to analyze and gradually modify original syntax given in IEEE 1364-2005 and to generate the input files for the parser generators. 

![alt text](http://software.rochus-keller.info/VerilogEbnf_screenshot.png "VerilogEbnf Screenshot")

### Build Steps
Follow these steps if you intend to build VerilogEbnf:

1. Create a directory; let's call it BUILD_DIR
2. Download the VerilogEbnf source code from https://github.com/rochus-keller/VerilogEbnf/archive/master.zip and unpack it to the BUILD_DIR; rename the subdirectory to "VerilogEbnf".
3. Download the NAF source code from https://github.com/rochus-keller/NAF/archive/master.zip and unpack it to the BUILD_DIR; rename "NAF-Master" to "NAF". We only need the Gui2 subdirectory so you can delete all other stuff in the NAF directory.
4. Download the Verilog source code from https://github.com/rochus-keller/Verilog/archive/master.zip and unpack it to the BUILD_DIR; rename the subdirectory to "Verilog". Note that there is a bootstrapping relation between Verilog and VerilogEbnf.
5. Goto the BUILD_DIR/VerilogEbnf subdirectory and execute `QTDIR/bin/qmake VerilogEbnf.pro` (see the Qt documentation concerning QTDIR).
6. Run make; after a couple of seconds you will find the executable in the tmp subdirectory.

Alternatively you can open VerilogEbnf.pro using QtCreator and build it there.

## Support
If you need support or would like to post issues or feature requests please use the Github issue list at https://github.com/rochus-keller/VerilogEbnf/issues or send an email to the author.



