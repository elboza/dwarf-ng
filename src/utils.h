/*
 utils.c : miscellaneous definitions.

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
typedef enum {comp_fixed,comp_var,comp_discardable} rl_comp;
struct comp_list
{
	char *s;
	int type;
	struct comp_list *prev,*next;
}*first_comp,*last_comp;
struct output_format
{
	int count;
	char out;
};
void file_probe(void);
void file_open(char *s);
void die(char *s);
char* rl_gets(char *prompt);
void execute(char *s);
void file_close(void);
void shell(void);
void load_headers(void);
uint16_t get_data16(uint16_t data);
uint32_t get_data32(uint32_t data);
uint64_t get_data64(uint64_t data);
int get_offset(char *s,char p);
void get_format(char *s,struct output_format *fmt);
