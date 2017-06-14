/*
 repl.h : read evaluate print and loop, shell header.

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
#ifndef DW_REPL_H
#define DW_REPL_H

#define MAX_CMD 1024

int quit_shell;
void funny_shell_disclaimer(void);
void normal_shell_disclaimer(void);
int check_funny(void);
void execute(char *s);
void run_script(char *file);
void shell(void);

#endif
