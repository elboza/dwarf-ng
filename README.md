dwarf-ng
========

###-- a little and powerful object file manipulation tool.


**dwarf-ng** is a powerful object file manipulation tools in the spirit of _gdb_. with dwarf-ng you can read and edit all the file's section headers as well as the raw data. With dwarf-ng you can create and customize new file's header and it can be used as a compiler back-end to create executables/object files. dwarf-ng also permits to inject easily new headers and pieces of code/data into the file. dwarf-ng currently handles

* Raw
* ELF (Elf32, Elf64),
* PE (Portable executables PE32, PE+) and
* Mach-O (os x executables 32&64bit)

files format. In the future new files type will be added.

####LICENSE

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
    
    
## HOWTO INSTALL

```
$ ./autogen.sh
( $ ./configure --help=short     # prints all ./configure compiling options)
$ ./configure (or ./configure --enable-no-as-needed)
$ make
# sudo make install
```


######warnings:
* gcc version
in gcc version =>4.6.1 (on linux) it may fail compiling. to avoid compiling failure enable the --no-as-needed flag
by running the following configuration:
./configure --enable-no-as-needed

* readline/history

to take advantage of all the features of the readline/history library, make sure to use the GNU version 5.0 or above.
In Mac OS X the default readline is the BSD 4 version. you can substitute or soft link the GNU versions (sould be located at /opt/local/include/readline/ and /opt/local/lib/librealine.dylib).
