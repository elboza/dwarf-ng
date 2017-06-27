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

### demo

```
$ dwarf -i demo.exe
dwarf-ng Copyright (C) 2007-2011 Fernando Iazeolla.
This program comes with ABSOLUTELY NO WARRANTY.
This is free software (GNU GPLv2), and you are welcome to redistribute it
under GNU GPL version 2 (see <http://www.gnu.org/licenses/>).
entering shell-interactive mode...
dwarf> info
tcmd500.exe : 3239748 bytes, PE Porable Executable Intel 386 cpu 32 bit little endian
mzexe header: mz
pe header: pe
sections: sect[0..3]
dwarf> pp $mz
e_magic: 0x5a4d (23117)
e_cblp: 0x90 (144)
e_cp: 0x3 (3)
e_crlc: 0x0 (0)
e_cparhdr: 0x4 (4)
e_minalloc: 0x0 (0)
e_maxalloc: 0xffff (65535)
e_ss: 0x0 (0)
e_sp: 0xb8 (184)
e_csum: 0x0 (0)
e_ip: 0x0 (0)
e_cs: 0x0 (0)
e_lfarlc: 0x40 (64)
e_ovno: 0x0 (0)
e_res: 
e_oemid: 0x0 (0)
e_oeminfo: 0x0 (0)
e_res2: 
e_lfanew: 0xd0 (208)
dwarf> pp $pe
Signature: 0x4550 (17744)
* FileHeader
* OptionalHeader
dwarf> pp $pe::FileHeader
Machine: 0x14c (332)
NumberOfSections: 0x4 (4)
TimeDateStamp: 0x3bd86c3f (1004039231)
PointerToSymbolTable: 0x0 (0)
NumberOfSymbols: 0x0 (0)
SizeOfOptionalHeader: 0xe0 (224)
Characteristics: 0x50f (1295)
dwarf> pp $pe::OptionalHeader
Magic: 0x10b (267)
MajorLinkerVersion: 0x6 (6)
MinorLinkerVersion: 0x0 (0)
SizeOfCode: 0x2200 (8704)
SizeOfInitializedData: 0x1600 (5632)
SizeOfUninitializedData: 0x0 (0)
AddressOfEntryPoint: 0x21af (8623)
BaseOfCode: 0x1000 (4096)
BaseOfData: 0x4000 (16384)
ImageBase: 0x400000 (4194304)
SectionAlignment: 0x1000 (4096)
FileAlignment: 0x200 (512)
MajorOperatingSystemVersion: 0x4 (4)
MinorOperatingSystemVersion: 0x0 (0)
MajorImageVersion: 0x4 (4)
MinorImageVersion: 0x0 (0)
MajorSubsystemVersion: 0x4 (4)
MinorSubsystemVersion: 0x0 (0)
Reserved1: 0x0 (0)
SizeOfImage: 0x7000 (28672)
SizeOfHeaders: 0x400 (1024)
CheckSum: 0x0 (0)
Subsystem: 0x2 (2)
DllCharacteristics: 0x0 (0)
SizeOfStackReserve: 0x100000 (1048576)
SizeOfStackCommit: 0x1000 (4096)
SizeOfHeapReserve: 0x100000 (1048576)
SizeOfHeapCommit: 0x1000 (4096)
LoaderFlags: 0x0 (0)
NumberOfRvaAndSizes: 0x10 (16)
dwarf> 

```

```
$ dwarf -i
dwarf-ng Copyright (C) 2007-2011 Fernando Iazeolla.
This program comes with ABSOLUTELY NO WARRANTY.
This is free software (GNU GPLv2), and you are welcome to redistribute it
under GNU GPL version 2 (see <http://www.gnu.org/licenses/>).
entering shell-interactive mode...
dwarf> open dwarf
507+1 records in
507+1 records out
259936 bytes (260 kB, 254 KiB) copied, 0,0101119 s, 25,7 MB/s
dwarf opened.
dwarf> info
dwarf : 259936 bytes, ELF Intel 80386 cpu 32 bit little endian
main header: elf
program header: ph [0..8]
section header: sh [0..35]
dwarf> pp $elf
e_ident: ELF�
e_type: 0x2 (2)
e_machine: 0x3 (3)
e_version: 0x1 (1)
e_entry: 0x8049130 (134517040)
e_phoff: 0x34 (52)
e_shoff: 0x3f1c0 (258496)
e_flags: 0x0 (0)
e_ehsize: 0x34 (52)
e_phentsize: 0x20 (32)
e_phnum: 0x9 (9)
e_shentsize: 0x28 (40)
e_shnum: 0x24 (36)
e_shstrndx: 0x21 (33)
dwarf> pp $ph
ph[0] [PT_PHDR] 0x34
ph[1] [PT_INTERP] 0x154
ph[2] [PT_LOAD] 0x0
ph[3] [PT_LOAD] 0x28ef8
ph[4] [PT_DYNAMIC] 0x28f04
ph[5] [PT_NOTE] 0x168
ph[6] [ ] 0x25564
ph[7] [ ] 0x0
ph[8] [ ] 0x28ef8
dwarf> pp $ph[5]
p_type: 0x4 (4) [PT_NOTE]
p_flags: 0x4 (4)
p_offset: 0x168 (360)
p_vaddr: 0x8048168 (134513000)
p_paddr: 0x8048168 (134513000)
p_filesz: 0x44 (68)
p_memsz: 0x44 (68)
p_align: 0x4 (4)
dwarf> pp $sh
sh[0] [] 0x0
sh[1] [.interp] 0x154
sh[2] [.note.ABI-tag] 0x168
sh[3] [.note.gnu.build-id] 0x188
sh[4] [.gnu.hash] 0x1ac
sh[5] [.dynsym] 0x218
sh[6] [.dynstr] 0x6f8
sh[7] [.gnu.version] 0xa20
sh[8] [.gnu.version_r] 0xabc
sh[9] [.rel.dyn] 0xb3c
sh[10] [.rel.plt] 0xb8c
sh[11] [.init] 0xd54
sh[12] [.plt] 0xd80
sh[13] [.plt.got] 0x1120
sh[14] [.text] 0x1130
sh[15] [.fini] 0x1ddc4
sh[16] [.rodata] 0x1dde0
sh[17] [.eh_frame_hdr] 0x25564
sh[18] [.eh_frame] 0x25e38
sh[19] [.init_array] 0x28ef8
sh[20] [.fini_array] 0x28efc
sh[21] [.jcr] 0x28f00
sh[22] [.dynamic] 0x28f04
sh[23] [.got] 0x28ffc
sh[24] [.got.plt] 0x29000
sh[25] [.data] 0x29100
sh[26] [.bss] 0x29370
sh[27] [.comment] 0x29370
sh[28] [.debug_aranges] 0x293a4
sh[29] [.debug_info] 0x29584
sh[30] [.debug_abbrev] 0x32ed8
sh[31] [.debug_line] 0x34b04
sh[32] [.debug_str] 0x39e08
sh[33] [.shstrtab] 0x3f076
sh[34] [.symtab] 0x3b94c
sh[35] [.strtab] 0x3da4c
dwarf> du 0
0000000000  7f 45 4c 46 01 01 01 00-00 00 00 00 00 00 00 00  .ELF............
0000000010  02 00 03 00 01 00 00 00-30 91 04 08 34 00 00 00  ........0...4...
0000000020  c0 f1 03 00 00 00 00 00-34 00 20 00 09 00 28 00  ........4. ...(.
0000000030  24 00 21 00 06 00 00 00-34 00 00 00 34 80 04 08  $.!.....4...4...
0000000040  34 80 04 08 20 01 00 00-20 01 00 00 05 00 00 00  4... ... .......
0000000050  04 00 00 00 03 00 00 00-54 01 00 00 54 81 04 08  ........T...T...
0000000060  54 81 04 08 13 00 00 00-13 00 00 00 04 00 00 00  T...............
0000000070  01 00 00 00 01 00 00 00-00 00 00 00 00 80 04 08  ................
0000000080  00 80 04 08 a0 82 02 00-a0 82 02 00 05 00 00 00  ................
dwarf> open shlang.output
80+1 records in
80+1 records out
41455 bytes (41 kB, 40 KiB) copied, 0,00371853 s, 11,1 MB/s
shlang.output opened.
dwarf> info
shlang.output : 41455 bytes, unknown file type.
dwarf> fl
  0: dwarf
* 1: shlang.output
dwarf> pp $cfg
writable: 1 yes
can_grow: 1 yes
name :shlang.output
work_on_copy: 1 yes
changed_altered: 0 no
verbose: 0 no
cpu_endian: 0 little endian
file_endian: 0 little endian
file_type: 0 Unknown
file_bit_class: 0 0bit
filesize: 41455
faddr: 0xb77af000
copydir: /tmp
copyname: /tmp/dw_temp_filex_0
fd_type: 1 FD_REGULAR_FILE
fd: 3
prev: (nil) next: (nil) first: 0x976f1e8 last: 0x976f1e8
dwarf> pp $maincfg
writable: 0 no
can_grow: 0 no
name :
work_on_copy: 1 yes
changed_altered: 0 no
verbose: 0 no
cpu_endian: 0 little endian
file_endian: 0 little endian
file_type: 0 Unknown
file_bit_class: 0 0bit
filesize: 0
faddr: (nil)
copydir: /tmp
copyname: dw_temp_filex
fd_type: 1 FD_REGULAR_FILE
fd: 0
prev: (nil) next: (nil) first: 0x976f1e8 last: 0x976f1e8
dwarf> fu 0
Ok. Now working on dwarf file.
** dwarf> info
dwarf : 259936 bytes, ELF Intel 80386 cpu 32 bit little endian
main header: elf
program header: ph [0..8]
section header: sh [0..35]
dwarf> quit
$ 

```


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
    
