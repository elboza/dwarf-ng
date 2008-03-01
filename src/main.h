/*
 main.h : dwarf's main header.

 (c) 2007-2008 Fernando Iazeolla

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
int ilook_debug; /* if set perform the debug function only*/
#define MAX_FILENAME 255
#define MAX_STR 255
char filename[MAX_FILENAME],*cmd;
int forced;  /*if set force a file to be treat as a given kind reguardless of what really kind is*/
int file_type;
int fd;
int mx;
void *faddr;

//file types
#define FT_NULL		0
#define FT_ELF		1
#define FT_MZ		2
#define FT_MACHO	3
#define FT_PE		4

#define QUITTING	123
