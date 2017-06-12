/*
 pe.h : PE file format definitions.

 (c) 2007-2011 Fernando Iazeolla

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
*/

#ifndef DW_PE_H
#define DW_PE_H

#define IMAGE_DOS_SIGNATURE                 0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE                 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE              0x454C      // LE
#define IMAGE_VXD_SIGNATURE                 0x454C      // LE
#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00

typedef struct {      // DOS .EXE header
    uint16_t   e_magic;                     // Magic number
    uint16_t   e_cblp;                      // Bytes on last page of file
    uint16_t   e_cp;                        // Pages in file
    uint16_t   e_crlc;                      // Relocations
    uint16_t   e_cparhdr;                   // Size of header in paragraphs
    uint16_t   e_minalloc;                  // Minimum extra paragraphs needed
    uint16_t   e_maxalloc;                  // Maximum extra paragraphs needed
    uint16_t   e_ss;                        // Initial (relative) SS value
    uint16_t   e_sp;                        // Initial SP value
    uint16_t   e_csum;                      // Checksum
    uint16_t   e_ip;                        // Initial IP value
    uint16_t   e_cs;                        // Initial (relative) CS value
    uint16_t   e_lfarlc;                    // File address of relocation table
    uint16_t   e_ovno;                      // Overlay number
    uint16_t   e_res[4];                    // Reserved words
    uint16_t   e_oemid;                     // OEM identifier (for e_oeminfo)
    uint16_t   e_oeminfo;                   // OEM information; e_oemid specific
    uint16_t   e_res2[10];                  // Reserved words
    uint32_t   e_lfanew;                    // File address of new exe header
} _IMAGE_DOS_HEADER;
typedef struct {
    uint16_t    Machine;
    uint16_t    NumberOfSections;
    uint32_t   TimeDateStamp;
    uint32_t   PointerToSymbolTable;
    uint32_t   NumberOfSymbols;
    uint16_t    SizeOfOptionalHeader;
    uint16_t    Characteristics;
}_IMAGE_FILE_HEADER ;
typedef struct {
    uint32_t   VirtualAddress;
    uint32_t   Size;
}_IMAGE_DATA_DIRECTORY ;
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16
typedef struct {
    //
    // Standard fields.
    //

    uint16_t    Magic;			//0x010b =32bit
    uint8_t    MajorLinkerVersion;
    uint8_t    MinorLinkerVersion;
    uint32_t   SizeOfCode;
    uint32_t   SizeOfInitializedData;
    uint32_t   SizeOfUninitializedData;
    uint32_t   AddressOfEntryPoint;
    uint32_t   BaseOfCode;
    uint32_t   BaseOfData;

    //
    // NT additional fields.
    //

    uint32_t   ImageBase;
    uint32_t   SectionAlignment;
    uint32_t   FileAlignment;
    uint16_t    MajorOperatingSystemVersion;
    uint16_t    MinorOperatingSystemVersion;
    uint16_t    MajorImageVersion;
    uint16_t    MinorImageVersion;
    uint16_t    MajorSubsystemVersion;
    uint16_t    MinorSubsystemVersion;
    uint32_t   Reserved1;
    uint32_t   SizeOfImage;
    uint32_t   SizeOfHeaders;
    uint32_t   CheckSum;
    uint16_t    Subsystem;
    uint16_t    DllCharacteristics;
    uint32_t   SizeOfStackReserve;
    uint32_t   SizeOfStackCommit;
    uint32_t   SizeOfHeapReserve;
    uint32_t   SizeOfHeapCommit;
    uint32_t   LoaderFlags;
    uint32_t   NumberOfRvaAndSizes;
    _IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
}_IMAGE_OPTIONAL_HEADER ;
typedef struct {
    uint16_t        Magic;		//0x20b =64bit
    uint8_t        MajorLinkerVersion;
    uint8_t        MinorLinkerVersion;
    uint32_t       SizeOfCode;
    uint32_t       SizeOfInitializedData;
    uint32_t       SizeOfUninitializedData;
    uint32_t       AddressOfEntryPoint;
    uint32_t       BaseOfCode;
    uint64_t      ImageBase;
    uint32_t       SectionAlignment;
    uint32_t       FileAlignment;
    uint16_t        MajorOperatingSystemVersion;
    uint16_t        MinorOperatingSystemVersion;
    uint16_t        MajorImageVersion;
    uint16_t        MinorImageVersion;
    uint16_t        MajorSubsystemVersion;
    uint16_t        MinorSubsystemVersion;
    uint32_t       Win32VersionValue;
    uint32_t       SizeOfImage;
    uint32_t       SizeOfHeaders;
    uint32_t       CheckSum;
    uint16_t        Subsystem;
    uint16_t        DllCharacteristics;
    uint64_t   SizeOfStackReserve;
    uint64_t   SizeOfStackCommit;
    uint64_t   SizeOfHeapReserve;
    uint64_t   SizeOfHeapCommit;
    uint32_t       LoaderFlags;
    uint32_t       NumberOfRvaAndSizes;
    _IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} _IMAGE_OPTIONAL_HEADER64;
typedef struct  {
    uint32_t Signature;
    _IMAGE_FILE_HEADER FileHeader;
    _IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} _IMAGE_NT_HEADERS64;
typedef struct {
    uint32_t Signature;
    _IMAGE_FILE_HEADER FileHeader;
    _IMAGE_OPTIONAL_HEADER OptionalHeader;
}_IMAGE_NT_HEADERS ;
#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct {
    uint8_t    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            uint32_t   PhysicalAddress;
            uint32_t   VirtualSize;
    } Misc;
    uint32_t   VirtualAddress;
    uint32_t   SizeOfRawData;
    uint32_t   PointerToRawData;
    uint32_t   PointerToRelocations;
    uint32_t   PointerToLinenumbers;
    uint16_t    NumberOfRelocations;
    uint16_t    NumberOfLinenumbers;
    uint32_t   Characteristics;
}_IMAGE_SECTION_HEADER ;

#define IMAGE_FILE_MACHINE_UNKNOWN		0x0	//The contents of this field are assumed to be applicable to any machine type
#define IMAGE_FILE_MACHINE_AM33			0x1d3	//Matsushita AM33
#define IMAGE_FILE_MACHINE_AMD64		0x8664	//x64
#define IMAGE_FILE_MACHINE_ARM			0x1c0	//ARM little endian
#define IMAGE_FILE_MACHINE_EBC			0xebc	//EFI byte code
#define IMAGE_FILE_MACHINE_I386			0x14c	//Intel 386 or later processors and compatible processors
#define IMAGE_FILE_MACHINE_IA64			0x200	//Intel Itanium processor family
#define IMAGE_FILE_MACHINE_M32R			0x9041	//Mitsubishi M32R little endian
#define IMAGE_FILE_MACHINE_MIPS16		0x266	//MIPS16
#define IMAGE_FILE_MACHINE_MIPSFPU		0x366	//MIPS with FPU
#define IMAGE_FILE_MACHINE_MIPSFPU16	0x466	//MIPS16 with FPU
#define IMAGE_FILE_MACHINE_POWERPC		0x1f0	//Power PC little endian
#define IMAGE_FILE_MACHINE_POWERPCFP	0x1f1	//Power PC with floating point support
#define IMAGE_FILE_MACHINE_R4000		0x166	//MIPS little endian
#define IMAGE_FILE_MACHINE_SH3			0x1a2	//Hitachi SH3
#define IMAGE_FILE_MACHINE_SH3DSP		0x1a3	//Hitachi SH3 DSP
#define IMAGE_FILE_MACHINE_SH4			0x1a6	//Hitachi SH4
#define IMAGE_FILE_MACHINE_SH5			0x1a8	//Hitachi SH5
#define IMAGE_FILE_MACHINE_THUMB		0x1c2	//Thumb
#define IMAGE_FILE_MACHINE_WCEMIPSV2	0x169	//MIPS little-endian WCE v2

#endif

