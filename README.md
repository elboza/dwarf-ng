dwarf-ng
========

### -- a little and powerful object file manipulation tool.


**dwarf-ng** is a powerful object file manipulation tools in the spirit of _gdb_. with dwarf-ng you can read and edit all the file's section headers as well as the raw data. With dwarf-ng you can create and customize new file's header and it can be used as a compiler back-end to create executables/object files. dwarf-ng also permits to inject easily new headers and pieces of code/data into the file. dwarf-ng currently handles

* Raw
* ELF (Elf32, Elf64),
* PE (Portable executables PE32, PE+) and
* Mach-O (os x executables 32&64bit)

files format. In the future new files type will be added.


## HOWTO INSTALL

```
$ make help             #(show make options)
$ make                  #(make FLEX=osx #to build in macosx)
$ sudo make install
```

## Dependencies

* flex
* bison
* readline (libreadline libreadline-dev)
* gcc


#### LICENSE

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
    
