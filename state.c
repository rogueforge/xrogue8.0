/*
    state.c - Portable Rogue Save State Code
    
    Copyright (C) 1999 Nicholas J. Kisseberth
   
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name(s) of the author(s) nor the names of other contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#define RSXR_STATS        0xABCD0001
#define RSXR_THING        0xABCD0002
#define RSXR_OBJECT       0xABCD0003
#define RSXR_MAGICITEMS   0xABCD0004
#define RSXR_KNOWS        0xABCD0005
#define RSXR_GUESSES      0xABCD0006
#define RSXR_OBJECTLIST   0xABCD0007
#define RSXR_BAGOBJECT    0xABCD0008
#define RSXR_MONSTERLIST  0xABCD0009
#define RSXR_MONSTERSTATS 0xABCD000A
#define RSXR_MONSTERS     0xABCD000B
#define RSXR_TRAP         0xABCD000C
#define RSXR_WINDOW       0xABCD000D
#define RSXR_DAEMONS      0xABCD000E
#define RSXR_IWEAPS       0xABCD000F
#define RSXR_IARMOR       0xABCD0010
#define RSXR_SPELLS       0xABCD0011
#define RSXR_ILIST        0xABCD0012
#define RSXR_HLIST        0xABCD0013
#define RSXR_DEATHTYPE    0xABCD0014
#define RSXR_CTYPES       0XABCD0015
#define RSXR_COORDLIST    0XABCD0016
#define RSXR_ROOMS        0XABCD0017

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include "rogue.h"

#define READSTAT ((format_error == 0) && (read_error == 0))
#define WRITESTAT (write_error == 0)

int read_error   = FALSE;
int write_error  = FALSE;
int format_error = FALSE;


int save_debug = TRUE;
#define DBG(x)   {if (save_debug) rsPrintf x;}

void
rsPrintf(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
/*  vfprintf(stderr,fmt, ap);*/
    va_end(ap);	
}


void *
get_list_item(struct linked_list *l, int i)
{
    int count = 0;

    while(l != NULL)
    {   
        if (count == i)
            return(l->l_data);
                        
        l = l->l_next;
        
        count++;
    }
    
    return(NULL);
}

int
rs_write(FILE *savef, void *ptr, int size)
{
    if (!write_error)
        ENCWRITE(ptr,size,fileno(savef));
		
    return(WRITESTAT);
}

int
rs_read(int inf, void *ptr, int size)
{
    if (!read_error && !format_error)
        ENCREAD(ptr, size, inf);
        
    if (read_error){
        printf("read error has occurred. restore short-circuited.\n");abort();}
    if (format_error)
        {printf("game format invalid. restore short-circuited.\n");abort();}

    return(READSTAT);
}

int
rs_write_int(FILE *savef, int c)
{
    rs_write(savef, &c, sizeof(int));
	DBG(("%d\n",c));    
    return(WRITESTAT);
}

int
rs_write_ulong(FILE *savef, unsigned long c)
{
    rs_write(savef, &c, sizeof(unsigned long));
	DBG(("%d\n",c));    
    return(WRITESTAT);
}

int
rs_write_boolean(FILE *savef, bool c)
{
    rs_write(savef, &c, sizeof(bool));
	DBG(("%d\n",c));    
    return(WRITESTAT);
}

int
rs_read_int(int inf, int *i)
{
    rs_read(inf, i, sizeof(int));
    return(READSTAT);
}

int
rs_read_ulong(int inf, unsigned long *i)
{
    rs_read(inf, i, sizeof(unsigned long));
    return(READSTAT);
}

int
rs_read_boolean(int inf, bool *i)
{
    rs_read(inf, i, sizeof(bool));
    return(READSTAT);
}

int
rs_write_ints(FILE *savef, int *c, int count)
{
    int n=0;

    rs_write_int(savef,count);
    
    for(n=0;n<count;n++)
        rs_write_int(savef,c[n]);

    return(WRITESTAT);
}

int
rs_write_short(FILE *savef, short c)
{
    rs_write(savef, &c, sizeof(short));
	DBG(("%d\n",c));
    return(WRITESTAT);
}

int
rs_write_shorts(FILE *savef, short *c, int count)
{
    int n=0;

    rs_write_int(savef,count);
    
    for(n=0;n<count;n++)
        rs_write_short(savef,c[n]);

    return(WRITESTAT);
}

int
rs_write_longs(FILE *savef, long *c, int count)
{
    int n=0;

    rs_write_int(savef,count);
    
    for(n=0;n<count;n++)
        rs_write_long(savef,c[n]);

    return(WRITESTAT);
}

int
rs_write_ulongs(FILE *savef, unsigned long *c, int count)
{
    int n=0;

    rs_write_int(savef,count);
    
    for(n=0;n<count;n++)
        rs_write_ulong(savef,c[n]);

    return(WRITESTAT);
}

int
rs_write_booleans(FILE *savef, bool *c, int count)
{
    int n=0;

    rs_write_int(savef,count);
    
    for(n=0;n<count;n++)
        rs_write_boolean(savef,c[n]);

    return(WRITESTAT);
}

int
rs_read_ints(int inf, int *i, int count)
{
    int n=0,value=0;
    
    if (rs_read_int(inf,&value) != 0)
    {
        if (value != count)
            format_error = TRUE;
        else
        {
            for(n=0;n<value;n++)
                rs_read_int(inf, &i[n]);
        }
    }
    
    return(READSTAT);
}

int
rs_read_shorts(int inf, short *i, int count)
{
    int n=0,value=0;
    
    if (rs_read_int(inf,&value) != 0)
    {
        if (value != count)
            format_error = TRUE;
        else
        {
            for(n=0;n<value;n++)
                rs_read_short(inf, &i[n]);
        }
    }
    
    return(READSTAT);
}

int
rs_read_longs(int inf, long *i, int count)
{
    int n=0,value=0;
    
    if (rs_read_int(inf,&value) != 0)
    {
        if (value != count)
            format_error = TRUE;
        else
        {
            for(n=0;n<value;n++)
                rs_read_long(inf, &i[n]);
        }
    }
    
    return(READSTAT);
}

int
rs_read_ulongs(int inf, unsigned long *i, int count)
{
    int n=0,value=0;
    
    if (rs_read_int(inf,&value) != 0)
    {
        if (value != count)
            format_error = TRUE;
        else
        {
            for(n=0;n<value;n++)
                rs_read_ulong(inf, &i[n]);
        }
    }
    
    return(READSTAT);
}

int
rs_read_booleans(int inf, bool *i, int count)
{
    int n=0,value=0;
    
    if (rs_read_int(inf,&value) != 0)
    {
        if (value != count)
            format_error = TRUE;
        else
        {
            for(n=0;n<value;n++)
                rs_read_boolean(inf, &i[n]);
        }
    }
    
    return(READSTAT);
}

int
rs_read_short(int inf, short *s)
{
    rs_read(inf, s, sizeof(short));
    
    return(READSTAT);
}

int
rs_write_long(FILE *savef, long c)
{
    rs_write(savef, &c, sizeof(long));
	DBG(("%d",c));    
    return(WRITESTAT);
}

int
rs_read_long(int inf, long *l)
{
    rs_read(inf, l, sizeof(long));
    
    return(READSTAT);
}

int
rs_write_levtype(FILE *savef, LEVTYPE c)
{
    int lt;
	
    switch(c)
	{
		case NORMLEV: lt = 1; break;
		case POSTLEV: lt = 2; break;
		case MAZELEV: lt = 3; break;
		case OUTSIDE: lt = 4; break;
		case STARTLEV: lt = 5; break;
		default: lt = -1; break;
	}
	
	rs_write_int(savef,lt);
		
    return(WRITESTAT);
}

int
rs_read_levtype(int inf, LEVTYPE *l)
{
    int lt;
	
    rs_read_int(inf, &lt);
	
    switch(lt)
	{
		case 1: *l = NORMLEV; break;
		case 2: *l = POSTLEV; break;
		case 3: *l = MAZELEV; break;
		case 4: *l = OUTSIDE; break;
		case 5: *l = STARTLEV; break;
		default: *l = -1; break;
	}
    
    return(READSTAT);
}

int
rs_write_char(FILE *savef, char c)
{
    rs_write(savef, &c, sizeof(char));
	DBG(("%c",c));    
    
    return(WRITESTAT);
}

int
rs_read_char(int inf, char *c)
{
    rs_read(inf, c, sizeof(char));
    
    return(READSTAT);
}

int
rs_write_string(FILE *savef, char *s)
{
    int len = 0;

    len = (s == NULL) ? 0L : strlen(s) + 1;

    rs_write_int(savef, len);

    if (len > 0)
       rs_write(savef, s, len);
    
    return(WRITESTAT);
}

int
rs_read_string_index(int inf, char master[][15], int maxindex, char **str)
{
    int i;

    if (rs_read_int(inf,&i) != 0)
    {
        if (i > maxindex)
        {
            printf("String index is out of range. %d > %d\n",      
                i, maxindex);
            printf("Sorry, invalid save game format\n");
            format_error = TRUE;
        }
        else if (i >= 0)
            *str = master[i];
        else
            *str = NULL;
    }
    return(READSTAT);
}

int
rs_write_string_index(FILE *savef, char master[][15], int maxindex, char *str)
{
    int i;

    for(i = 0; i < maxindex; i++)
    {
        if (str == master[i])
        {
            rs_write_int(savef,i);
            return(WRITESTAT);
        }
    }

    rs_write_int(savef,-1);
    return(WRITESTAT);
}
 
int
rs_read_scrolls(int inf)
{
    int i;

    for(i = 0; i < MAXSCROLLS; i++)
    {
        rs_read_new_string(inf,&s_names[i]);
        rs_read_boolean(inf,&s_know[i]);
        rs_read_new_string(inf,&s_guess[i]);
    }

    return(READSTAT);
}

int
rs_write_scrolls(FILE *savef)
{
    int i;

    for(i = 0; i < MAXSCROLLS; i++)
    {
        rs_write_string(savef,s_names[i]);
        rs_write_boolean(savef,s_know[i]);
        rs_write_string(savef,s_guess[i]);
    }
    return(READSTAT);
}

int
rs_read_potions(int inf)
{
    int i;

    for(i = 0; i < MAXPOTIONS; i++)
    {
        rs_read_string_index(inf,rainbow,cNCOLORS,&p_colors[i]);
        rs_read_boolean(inf,&p_know[i]);
        rs_read_new_string(inf,&p_guess[i]);
    }

    return(READSTAT);
}

int
rs_write_potions(FILE *savef)
{
    int i;

    for(i = 0; i < MAXPOTIONS; i++)
    {
        rs_write_string_index(savef,rainbow,cNCOLORS,p_colors[i]);
        rs_write_boolean(savef,p_know[i]);
        rs_write_string(savef,p_guess[i]);
    }

    return(WRITESTAT);
}

int
rs_read_rings(int inf)
{
    int i;

    for(i = 0; i < MAXRINGS; i++)
    {
        rs_read_string_index(inf,stones,cNSTONES,&r_stones[i]);
        rs_read_boolean(inf,&r_know[i]);
        rs_read_new_string(inf,&r_guess[i]);
    }

    return(READSTAT);
}

int
rs_write_rings(FILE *savef)
{
    int i;

    for(i = 0; i < MAXRINGS; i++)
    {
        rs_write_string_index(savef,stones,cNSTONES,r_stones[i]);
        rs_write_boolean(savef,r_know[i]);
        rs_write_string(savef,r_guess[i]);
    }

    return(WRITESTAT);
}

int
rs_read_misc(int inf)
{
    int i;

    for(i = 0; i < MAXMM; i++)
    {
        rs_read_boolean(inf,&m_know[i]);
        rs_read_new_string(inf,&m_guess[i]);
    }

    return(READSTAT);
}

int
rs_write_misc(FILE *savef)
{
    int i;

    for(i = 0; i < MAXMM; i++)
    {
        rs_write_boolean(savef,m_know[i]);
        rs_write_string(savef,m_guess[i]);
    }
 
    return(WRITESTAT);
}
 
int
rs_write_sticks(FILE *savef)
{
    int i;

    for (i = 0; i < MAXSTICKS; i++)
    {
        if (strcmp(ws_type[i],"staff") == 0)
        {
            rs_write_int(savef,0);
            rs_write_string_index(savef, wood, cNWOOD, ws_made[i]);
        }
        else
        {
            rs_write_int(savef,1);
            rs_write_string_index(savef, metal, cNMETAL, ws_made[i]);
        }
        rs_write_boolean(savef, ws_know[i]);
        rs_write_string(savef, ws_guess[i]);
    }
 
    return(WRITESTAT);
}
        
int
rs_read_sticks(int inf)
{
    int i = 0, list = 0;

    for(i = 0; i < MAXSTICKS; i++)
    { 
        rs_read_int(inf,&list);
        if (list == 0)
        {
            rs_read_string_index(inf,wood,cNWOOD,&ws_made[i]);
            ws_type[i] = "staff";
        }
        else 
        {
            rs_read_string_index(inf,metal,cNMETAL,&ws_made[i]);
            ws_type[i] = "wand";
        }
        rs_read_boolean(inf, &ws_know[i]);
        rs_read_new_string(inf, &ws_guess[i]);
    }

    return(READSTAT);
}

int
rs_read_string(int inf, char *s, int max)
{
    int len = 0;

    if (rs_read_int(inf, &len) != FALSE)
    {   
        if (len > max)
        {
            printf("String too long to restore. %d > %d\n",
                len,max);abort();
            printf("Sorry, invalid save game format\n");
            format_error = TRUE;
        }
    
        rs_read(inf, s, len);
    }
    
    return(READSTAT);
}

int
rs_read_new_string(int inf, char **s)
{
    int len=0;
    char *buf=0;

    if (rs_read_int(inf, &len) != FALSE)
    {
        if (len == 0)
            *s = NULL;
        else
        {       
            buf = malloc(len);

            if (buf == NULL)            
                read_error = TRUE;
            else
            {
                rs_read(inf, buf, len);

                *s = buf;
            }
        }
    }
    
    return(READSTAT);
}

int
rs_write_strings(FILE *savef, char *s[], int count)
{
    int len = 0;
    int n = 0;

    rs_write_int(savef,count);

    for(n = 0; n < count; n++)
    {
        len = (s[n] == NULL) ? 0L : strlen(s[n]) + 1;
        rs_write_int(savef, len);
        rs_write(savef, s[n], len);
		DBG(("%s",s[n]));
    }
    
    return(WRITESTAT);
}

int
rs_write_words(FILE *savef, struct words *w, int count)
{
    int n = 0;

    rs_write_int(savef,count);

    for(n = 0; n < count; n++)
    {
        rs_write(savef, w[n].w_string, sizeof(w[n].w_string));
		DBG(("%s",w[n].w_string));
    }
    
    return(WRITESTAT);
}

int
rs_read_words(int inf, struct words *w, int count)
{
    int n = 0;
    int value = 0;

    rs_read_int(inf,&value);

    if (value != count)
    {
        printf("Incorrect number of words in block. %d != %d.",
                value,count);
        printf("Sorry, invalid save game format");
        format_error = TRUE;
    }
    else for(n = 0; n < count; n++)
    {
        rs_read(inf, w[n].w_string, sizeof(w[n].w_string));
    }
    
    return(READSTAT);
}

int
rs_read_new_strings(int inf, char **s, int count)
{
    int len   = 0;
    int n     = 0;
    int value = 0;
    
    if (rs_read_int(inf,&value) != 0)
    {
        if (value != count)
        {
            printf("Incorrect number of strings in block. %d > %d.",
                value,count);abort();
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else
            for(n=0; n<value; n++)
            {
                rs_read_int(inf, &len);
            
                if (len == 0)
                    s[n]=0;
                else 
                {
                    s[n] = malloc(len);
                    rs_read(inf,s[n],len);
                }
            }
    }
    
    return(READSTAT);
}

int
rs_write_coord(FILE *savef, coord *c)
{
    DBG(("X ="));
	rs_write_int(savef, c->x);
    DBG(("Y ="));
	rs_write_int(savef, c->y);
    
    return(WRITESTAT);
}

int
rs_read_coord(int inf, coord *c)
{
    rs_read_int(inf,&c->x);
    rs_read_int(inf,&c->y);
    
    return(READSTAT);
}

int
rs_write_daemons(FILE *savef, struct delayed_action *d_list,int count)
{
    int i = 0;
    int func = 0;
        
	DBG(("Daemons\n"));
    rs_write_int(savef, RSXR_DAEMONS);
    rs_write_int(savef, count);
        
    for(i=0; i < count; i++)
    {
        if (d_list[i].d_func == rollwand)
            func = 1;
        else if (d_list[i].d_func == doctor)
            func = 2;
        else if (d_list[i].d_func == stomach)
            func = 3;
        else if (d_list[i].d_func == trap_look)
            func = 4;
        else if (d_list[i].d_func == eat_gold)
            func = 5;
        else if (d_list[i].d_func == ring_search)
            func = 6;
        else if (d_list[i].d_func == ring_teleport)
            func = 7;
        else if (d_list[i].d_func == fumble)
            func = 8;
        else if (d_list[i].d_func == strangle)
            func = 9;
        else if (d_list[i].d_func == unconfuse)
            func = 10;
        else if (d_list[i].d_func == swander)
            func = 11;
        else if (d_list[i].d_func == spell_recovery)
            func = 12;
        else if (d_list[i].d_func == chant_recovery)
            func = 13;
        else if (d_list[i].d_func == prayer_recovery)
            func = 14;
        else if (d_list[i].d_func == cure_disease)
            func = 15;
        else if (d_list[i].d_func == unstink)
            func = 16;
        else if (d_list[i].d_func == res_strength)
            func = 17;
        else if (d_list[i].d_func == undance)
            func = 18;
        else if (d_list[i].d_func == suffocate)
            func = 19;
        else if (d_list[i].d_func == wghtchk)
            func = 20;
        else if (d_list[i].d_func == dust_appear)
            func = 21;
        else if (d_list[i].d_func == unchoke)
            func = 22;
        else if (d_list[i].d_func == sight)
            func = 23;
        else if (d_list[i].d_func == changeclass)
            func = 24;
        else if (d_list[i].d_func == cloak_charge)
            func = 25;
        else if (d_list[i].d_func == quill_charge)
            func = 26;
        else if (d_list[i].d_func == nohaste)
            func = 27;
        else if (d_list[i].d_func == noslow)
            func = 28;
        else if (d_list[i].d_func == unclrhead)
            func = 29;
        else if (d_list[i].d_func == unsee)
            func = 30;
        else if (d_list[i].d_func == unphase)
            func = 31;
        else if (d_list[i].d_func == land)
            func = 32;
        else if (d_list[i].d_func == appear)
            func = 33;
        else if (d_list[i].d_func == unskill)
            func = 34;
        else if (d_list[i].d_func == nofire)
            func = 35;
        else if (d_list[i].d_func == nocold)
            func = 36;
        else if (d_list[i].d_func == nobolt)
            func = 37;


        else
            func = 0;

        rs_write_int(savef, d_list[i].d_type);
        rs_write_int(savef, func);

        if (d_list[i].d_func == doctor)
           rs_write_int(savef, 1);
        else if (d_list[i].d_func == eat_gold)
        {
			int index;
			index = find_list_ptr(player.t_pack,d_list[i].d_arg);
			rs_write_int(savef,index);
        }
        else if (d_list[i].d_func == changeclass)
        {
            rs_write_long(savef, (long)d_list[i].d_arg);
        }
        else if (d_list[i].d_func == cloak_charge)
        {
			int index;
			index = find_list_ptr(player.t_pack,d_list[i].d_arg);
			rs_write_int(savef,index);
        }
        else
            rs_write_long(savef, (long)d_list[i].d_arg);

        rs_write_int(savef, d_list[i].d_time);
    }
    
    return(WRITESTAT);
}       

int
rs_read_daemons(int inf, struct delayed_action *d_list, int count)
{
    int i = 0;
    int func = 0;
    int value = 0;
    int id = 0;
	int dummy = 0;
    
    if (d_list == NULL)
        printf("HELP THERE ARE NO DAEMONS\n");
    
    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_DAEMONS)
        {
            printf("Invalid id. %x != %x(RSXR_DAEMONS)\n",
                id,RSXR_DAEMONS);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else if (rs_read_int(inf, &value) != 0)
        {
            if (value > count)
            {
                printf("Incorrect number of daemons in block. %d > %d.",
                    value,count);
                printf("Sorry, invalid save game format");
                format_error = TRUE;
            }
            else
            {
                for(i=0; i < value; i++)
                {
                    func = 0;
                    rs_read_int(inf, &d_list[i].d_type);
                    rs_read_int(inf, &func);
                    
                    switch(func)
                    {
                        case  1: d_list[i].d_func = rollwand;
                                 break;
                        case  2: d_list[i].d_func = doctor;
                                 break;
                        case  3: d_list[i].d_func = stomach;
                                 break;
                        case  4: d_list[i].d_func = trap_look;
                                 break;
                        case  5: d_list[i].d_func = eat_gold;
                                 break;
                        case  6: d_list[i].d_func = ring_search;
                                 break;
                        case  7: d_list[i].d_func = ring_teleport;
                                 break;
                        case  8: d_list[i].d_func = fumble;
                                 break;
                        case  9: d_list[i].d_func = strangle;
                                 break;
                        case 10: d_list[i].d_func = unconfuse;
                                 break;
                        case 11: d_list[i].d_func = swander;
                                 break;
                        case 12: d_list[i].d_func = spell_recovery;
                                 break;
                        case 13: d_list[i].d_func = chant_recovery;
                                 break;
                        case 14: d_list[i].d_func = prayer_recovery;
                                 break;
                        case 15: d_list[i].d_func = cure_disease;
                                 break;
                        case 16: d_list[i].d_func = unstink;
                                 break;
                        case 17: d_list[i].d_func = res_strength;
                                 break;
                        case 18: d_list[i].d_func = undance;
                                 break;
                        case 19: d_list[i].d_func = suffocate;
                                 break;
                        case 20: d_list[i].d_func = wghtchk;
                                 break;
                        case 21: d_list[i].d_func = dust_appear;
                                 break;
                        case 22: d_list[i].d_func = unchoke;
                                 break;
                        case 23: d_list[i].d_func = sight;
                                 break;
                        case 24: d_list[i].d_func = changeclass;
                                 break;
                        case 25: d_list[i].d_func = cloak_charge;
                                 break;
                        case 26: d_list[i].d_func = quill_charge;
                                 break;
                        case 27: d_list[i].d_func = nohaste;
                                 break;
                        case 28: d_list[i].d_func = noslow;
                                 break;
                        case 29: d_list[i].d_func = unclrhead;
                                 break;
                        case 30: d_list[i].d_func = unsee;
                                 break;
                        case 31: d_list[i].d_func = unphase;
                                 break;
                        case 32: d_list[i].d_func = land;
                                 break;
                        case 33: d_list[i].d_func = appear;
                                 break;
                        case 34: d_list[i].d_func = unskill;
                                 break;
                        case 35: d_list[i].d_func = nofire;
                                 break;
                        case 36: d_list[i].d_func = nocold;
                                 break;
                        case 37: d_list[i].d_func = nobolt;
                                 break;
                        default: d_list[i].d_func = NULL;
                                 break;
                    }

                    if (d_list[i].d_func == doctor)
                    {
                        rs_read_int(inf, &dummy);
                        d_list[i].d_arg = (void *)&player;
                    }
                    else if (d_list[i].d_func == eat_gold)
                    {
						rs_read_int(inf, &dummy);
						d_list[i].d_arg = get_list_item(player.t_pack,dummy);
						if (d_list[i].d_arg == NULL)
							d_list[i].d_type = 0;
                    }
                    else if (d_list[i].d_func == changeclass)
                    {
                        rs_read_long(inf, (long *)&d_list[i].d_arg);
                    }
                    else if (d_list[i].d_func == cloak_charge)
                    {
						rs_read_int(inf, &dummy);
						d_list[i].d_arg = get_list_item(player.t_pack,dummy);
						if (d_list[i].d_arg == NULL)
							d_list[i].d_type = 0;
                    }
                    else
                        rs_read_long(inf, (long *)&d_list[i].d_arg);

                    rs_read_int(inf, &d_list[i].d_time);
                }
            }
        }
    }
    
    return(READSTAT);
}       
        
int
rs_write_rooms(FILE *savef, struct room r[], int count)
{
    int n = 0,i = -1;
    struct linked_list *l;

	DBG(("Rooms\n"));
	rs_write_int(savef, RSXR_ROOMS);
    rs_write_int(savef, count);
    
    for(n=0; n<count; n++)
    {
        rs_write_coord(savef, &r[n].r_pos);
        rs_write_coord(savef, &r[n].r_max);
        rs_write_long(savef, r[n].r_flags);
        rs_write_coord_list(savef, r[n].r_exit);

        l = r[n].r_fires;
        rs_write_int(savef, list_size(l));

        while (l != NULL)
        {
            i = find_list_ptr(mlist,l->l_data);
            rs_write_int(savef,i);
            l = l->l_next;
        }
    }
    return(WRITESTAT);
}

int
rs_read_rooms(int inf, struct room *r, int count)
{
    int value = 0, n = 0, i = 0, index = 0, id = 0;
    struct linked_list *fires=NULL, *item;

    if (rs_read_int(inf,&id) != 0)
	{
        if (id != RSXR_ROOMS)
        {
            printf("Invalid id. %x != %x(RSXR_ROOMS)\n",
                id,RSXR_ROOMS);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
		else if (rs_read_int(inf,&value) != 0)
        {
	        if (value != count)
	        {
    	        printf("Incorrect number of rooms in block. %d > %d.",
        	        value,count);
    	        printf("Sorry, invalid save game format");
        	    format_error = TRUE;
	        }
    	    else
            {
                for(n=0; n<value; n++)
    	        {   
        	    	rs_read_coord(inf,&r[n].r_pos);
            	    rs_read_coord(inf,&r[n].r_max);
	            	rs_read_long(inf,&r[n].r_flags);
  	            	rs_read_coord_list(inf, &r[n].r_exit);

                    rs_read_int(inf, &i);

    	            while (i>0)
                	{
        		        rs_read_int(inf,&index);
                    	
						if (index >= 0)
	                    {
						    void *data;
        	                data = get_list_item(mlist,index);
            	            item = new_item(sizeof(struct linked_list *));
                	        item->l_data = data;
                    	    if (fires == NULL)
								fires = item;
							else
							    attach(fires,item);
	                    }
    	                i--;
        	        }
					r[n].r_fires=fires;
            	}
        	}
		}
    }

    return(READSTAT);
}

int
rs_write_object(FILE *savef, struct object *o)
{
	save_debug = FALSE;
    rs_write_int(savef, RSXR_OBJECT);
    rs_write_int(savef, o->o_type);
    rs_write_coord(savef, &o->o_pos);
    rs_write_char(savef, o->o_launch);
    rs_write_string(savef, o->o_damage);
    rs_write_string(savef, o->o_hurldmg);
    rs_write_int(savef, o->o_count);
    rs_write_int(savef, o->o_which);
    rs_write_int(savef, o->o_hplus);
    rs_write_int(savef, o->o_dplus);
    rs_write_int(savef, o->o_ac);
    rs_write_long(savef, o->o_flags);
    rs_write_int(savef, o->o_group);
    rs_write_int(savef, o->o_weight);
    rs_write(savef, o->o_mark, MARKLEN);
	save_debug = TRUE;

	DBG(("Object\n"));
	DBG(("    SaveID  : %X\n",RSXR_OBJECT));
	DBG(("    Type    : %d\n",o->o_type));
	DBG(("    Pos     : %d %d\n",o->o_pos.x,o->o_pos.y));
	DBG(("    Launch  : %c\n",o->o_launch));
	DBG(("    Damage  : %s\n",o->o_damage));
	DBG(("    Hurl    : %s\n",o->o_hurldmg));
	DBG(("    Count   : %d\n",o->o_count));
	DBG(("    Which   : %d\n",o->o_which));
	DBG(("    HPlus   : %d\n",o->o_hplus));
	DBG(("    DPlus   : %d\n",o->o_dplus));
	DBG(("    AC      : %d\n",o->o_ac));
	DBG(("    Flags   : %X\n",o->o_flags));
	DBG(("    Group   : %d\n",o->o_group));
	DBG(("    Weight  : %d\n",o->o_weight));
	DBG(("    Mark    : %s\n",o->o_mark));
	if (o->contents == NULL)
	{
		DBG(("    Contents: None\n"));
		save_debug = FALSE;
	}
	else
	{
		DBG(("    CONTENTS\n"));
	}
		
    rs_write_object_list(savef, o->contents);
	
	if (o->contents != NULL)
		DBG(("    END_CONTENTS\n"));
	
	save_debug = TRUE;
    return(WRITESTAT);
}

int
rs_read_object(int inf, struct object *o)
{
    int id;

    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_OBJECT)
        {
            printf("Invalid id. %x != %x(RSXR_OBJECT)\n", id,RSXR_OBJECT);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else
        {
            rs_read_int(inf, &o->o_type);
            rs_read_coord(inf, &o->o_pos);
            rs_read_char(inf, &o->o_launch);
            rs_read_string(inf, o->o_damage,sizeof(o->o_damage));
            rs_read_string(inf, o->o_hurldmg,sizeof(o->o_hurldmg));
            rs_read_int(inf, &o->o_count);
            rs_read_int(inf, &o->o_which);
            rs_read_int(inf, &o->o_hplus);
            rs_read_int(inf, &o->o_dplus);
            rs_read_int(inf,&o->o_ac);
            rs_read_long(inf,&o->o_flags);
            rs_read_int(inf,&o->o_group);
            rs_read_int(inf, &o->o_weight);
            rs_read(inf, o->o_mark, MARKLEN);
            rs_read_object_list(inf,&o->contents);

        }
    }

    return(READSTAT);
}

int
rs_write_stats(FILE *savef, struct stats *s)
{
	DBG(("Stats\n"));
    rs_write_int(savef, RSXR_STATS);
	
    rs_write_short(savef, s->s_str);
    rs_write_short(savef, s->s_intel);
    rs_write_short(savef, s->s_wisdom);
    rs_write_short(savef, s->s_dext);
    rs_write_short(savef, s->s_const);
    rs_write_short(savef, s->s_charisma);
    rs_write_ulong(savef, s->s_exp);
    rs_write_int(savef, s->s_lvladj);
    rs_write_int(savef, s->s_lvl);
    rs_write_int(savef, s->s_arm);
    rs_write_int(savef, s->s_hpt);
    rs_write_int(savef, s->s_pack);
    rs_write_int(savef, s->s_carry);
    rs_write_string(savef, s->s_dmg);

    return(WRITESTAT);
}

int
rs_read_stats(int inf, struct stats *s)
{
    int id;

    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_STATS)
        {
            printf("Invalid id. %x != %x(RSXR_STATS)\n", id,RSXR_STATS);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else
        {
		    rs_read_short(inf,&s->s_str);
		    rs_read_short(inf,&s->s_intel);
		    rs_read_short(inf,&s->s_wisdom);
		    rs_read_short(inf,&s->s_dext);
		    rs_read_short(inf,&s->s_const);
		    rs_read_short(inf,&s->s_charisma);
		    rs_read_ulong(inf,&s->s_exp);
		    rs_read_int(inf,&s->s_lvladj);
		    rs_read_int(inf,&s->s_lvl);
		    rs_read_int(inf,&s->s_arm);
		    rs_read_int(inf,&s->s_hpt);
		    rs_read_int(inf,&s->s_pack);
		    rs_read_int(inf,&s->s_carry);
		    rs_read_string(inf,s->s_dmg,sizeof(s->s_dmg));
		}
	}
	    
    return(READSTAT);
}

int
rs_write_mstats(FILE *savef, struct mstats *s)
{
	DBG(("M-Stats\n"));
    rs_write_int(savef, RSXR_STATS);
    rs_write_short(savef, s->ms_str);
    /*printf("    Strength: %d\n",s->ms_str);*/
    rs_write_short(savef, s->ms_dex);
    rs_write_short(savef, s->ms_move);
    rs_write_ulong(savef, s->ms_exp);
    rs_write_short(savef, s->ms_lvl);
    rs_write_short(savef, s->ms_arm);
    rs_write_string(savef, s->ms_hpt);
    rs_write_string(savef, s->ms_dmg);
    /*printf("    Damage: %s\n",s->ms_dmg);*/

    return(WRITESTAT);
}

int
rs_read_mstats(int inf, struct mstats *s)
{
    int id;

    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_STATS)
        {
            printf("Invalid id. %x != %x(RSXR_STATS)\n", id,RSXR_STATS);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
		else
		{
		    rs_read_short(inf,&s->ms_str);
			/*printf("    Strength: %d\n",s->ms_str);*/
		    rs_read_short(inf,&s->ms_dex);
			/*printf("    Dexterity: %d\n",s->ms_dex);*/
		    rs_read_short(inf,&s->ms_move);
			/*printf("    Moves: %d\n",s->ms_move);*/
		    rs_read_ulong(inf,&s->ms_exp);
			/*printf("    Experience: %d\n",s->ms_exp);*/
		    rs_read_short(inf,&s->ms_lvl);
			/*printf("    Level: %d\n",s->ms_lvl);*/
		    rs_read_short(inf,&s->ms_arm);
			/*printf("    Armor: %d\n",s->ms_arm);*/
		    rs_read_string(inf,s->ms_hpt,sizeof(s->ms_hpt));
			/*printf("    HP: %s\n",s->ms_hpt);*/
		    rs_read_string(inf,s->ms_dmg,sizeof(s->ms_dmg));
			/*printf("    Damage: %s\n",s->ms_dmg);*/
		}
	}
    
    return(READSTAT);
}

int
rs_write_init_weps(FILE *savef, struct init_weps *w, int count)
{
    int i;

	DBG(("Init-Weps\n"));
    rs_write_int(savef, RSXR_IWEAPS);
    rs_write_int(savef, count);

    for(i=0;i<count;i++)
    {
        rs_write_string(savef, w[i].w_name);
        rs_write_string(savef, w[i].w_dam);
        rs_write_string(savef, w[i].w_hrl);
        rs_write_char(savef, w[i].w_launch);
        rs_write_int(savef, w[i].w_flags);
        rs_write_int(savef, w[i].w_rate);
        rs_write_int(savef, w[i].w_wght);
        rs_write_int(savef, w[i].w_worth);
    }
    return(WRITESTAT);
}

int
rs_read_init_weps(int inf, struct init_weps *w,int count)
{
    int id,value,i;

    rs_read_int(inf, &id);
    rs_read_int(inf, &value);

    if (value != count)
    {
        printf("Incorrect number of init_weps in block. %d != %d.",
        	        value,count);
        printf("Sorry, invalid save game format");
   	    format_error = TRUE;
    }
    else for (i = 0; i < count; i++)
    {
        rs_read_string(inf, w[i].w_name, sizeof(w[i].w_name));
        rs_read_string(inf, w[i].w_dam, sizeof(w[i].w_dam));
        rs_read_string(inf, w[i].w_hrl, sizeof(w[i].w_hrl));
        rs_read_char(inf, &w[i].w_launch);
        rs_read_int(inf, &w[i].w_flags);
        rs_read_int(inf, &w[i].w_rate);
        rs_read_int(inf, &w[i].w_wght);
        rs_read_int(inf, &w[i].w_worth);
    }
    return(READSTAT);
}

int
rs_write_init_armor(FILE *savef, struct init_armor *a, int count)
{
    int i;
	DBG(("Init-Armor\n"));
    rs_write_int(savef, RSXR_IARMOR);
    rs_write_int(savef, count);
    for(i=0;i<count;i++)
    {
        rs_write_string(savef, a[i].a_name);
        rs_write_int(savef, a[i].a_prob);
        rs_write_int(savef, a[i].a_class);
        rs_write_int(savef, a[i].a_worth);
        rs_write_int(savef, a[i].a_wght);
    }
    return(WRITESTAT);
}

int
rs_read_init_armor(int inf, struct init_armor *a,int count)
{
    int id,value,i;

    rs_read_int(inf, &id);
    rs_read_int(inf, &value);

    for(i=0;i<count;i++)
    {
        rs_read_string(inf, a[i].a_name, sizeof(a[i].a_name));
        rs_read_int(inf, &a[i].a_prob);
        rs_read_int(inf, &a[i].a_class);
        rs_read_int(inf, &a[i].a_worth);
        rs_read_int(inf, &a[i].a_wght);
    }

    return(READSTAT);
}

int
rs_write_spells(FILE *savef, struct spells *s, int count)
{
    int i;
	DBG(("Spells\n"));
    rs_write_int(savef, RSXR_SPELLS);
    rs_write_int(savef, count);
    for(i=0;i<count;i++)
    {
        rs_write_short(savef, s[i].s_which);
        rs_write_short(savef, s[i].s_cost);
        rs_write_short(savef, s[i].s_type);
        rs_write_int(savef, s[i].s_flag);
    }
    return(WRITESTAT);
}

int
rs_read_spells(int inf, struct spells *s,int count)
{
    int id,value,i;

    rs_read_int(inf, &id);
    rs_read_int(inf, &value);

    for(i=0;i<count;i++)
    {
        rs_read_short(inf, &s[i].s_which);
        rs_read_short(inf, &s[i].s_cost);
        rs_read_short(inf, &s[i].s_type);
        rs_read_int(inf, &s[i].s_flag);
    }
    return(READSTAT);
}

int
rs_write_item_list(FILE *savef, struct item_list *i)
{
	DBG(("Item List\n"));
    rs_write_int(savef, RSXR_ILIST);
    rs_write_char(savef, i->item_ch);
    rs_write_string(savef, i->item_desc);
    return(WRITESTAT);
}

int
rs_read_item_list(int inf, struct item_list *i)
{
    int id;

    rs_read_int(inf, &id);

	rs_read_char(inf, &i->item_ch); 
	rs_read_string(inf, i->item_desc,sizeof(i->item_desc)); 
	return(READSTAT); 
}

int
rs_write_h_list(FILE *savef, struct h_list *h)
{
	DBG(("H List\n"));
    rs_write_int(savef, RSXR_HLIST);
    rs_write_char(savef, h->h_ch);
    rs_write_string(savef, h->h_desc);
    return(WRITESTAT);
}

int
rs_read_h_list(int inf, struct h_list *h)
{
    int id;

    rs_read_int(inf, &id);

    rs_read_char(inf, &h->h_ch);
    rs_read_string(inf, h->h_desc,sizeof(h->h_desc));
    return(READSTAT);
}

int
rs_write_death_types(FILE *savef, struct death_type *d,int count)
{
    int i;

	DBG(("Death Types\n"));
    rs_write_int(savef, RSXR_DEATHTYPE);
    rs_write_int(savef, count);

    for(i=0; i < count; i++)
    {
        rs_write_int(savef, d[i].reason);
        rs_write_string(savef, d[i].name);
    }
    return(WRITESTAT);
}

int
rs_read_death_types(int inf, struct death_type *d, int count)
{
    int id,value,i;

    rs_read_int(inf, &id);
    rs_read_int(inf, &value);
    if (value != count)
	{
        printf("Incorrect number of death_types in block. %d > %d.",
               value,count);
    	printf("Sorry, invalid save game format");
        format_error = TRUE;
    }
    else for(i=0;i < count;i++)
    {
        rs_read_int(inf, &d[i].reason);
        rs_read_string(inf, d[i].name,sizeof(d[i].name));
    }
    return(READSTAT);
}

int
rs_write_character_types(FILE *savef, struct character_types *c, int count)
{
    int i;

	DBG(("Character Types\n"));
    rs_write_int(savef, RSXR_CTYPES);
    rs_write_int(savef,count);

    for(i=0;i<count;i++)
    {
        rs_write_string(savef, c[i].name);
        rs_write_long(savef, c[i].start_exp);
        rs_write_long(savef, c[i].cap);
        rs_write_int(savef, c[i].hit_pts);
        rs_write_int(savef, c[i].base);
        rs_write_int(savef, c[i].max_lvl);
        rs_write_int(savef, c[i].factor);
        rs_write_int(savef, c[i].offset);
        rs_write_int(savef, c[i].range);
    }
    return(WRITESTAT);
}

int
rs_read_character_types(int inf, struct character_types *c,int count)
{
    int id,value,i;

    rs_read_int(inf, &id);
    rs_read_int(inf, &value);

    if (value != count)
	{
        printf("Incorrect number of rooms in block. %d > %d.",
               value,count);
    	printf("Sorry, invalid save game format");
        format_error = TRUE;
    }
    else for (i = 0; i < count; i++)
    {
        rs_read_string(inf, c[i].name,sizeof(c[i].name));
        rs_read_long(inf, &c[i].start_exp);
        rs_read_long(inf, &c[i].cap);
        rs_read_int(inf, &c[i].hit_pts);
        rs_read_int(inf, &c[i].base);
        rs_read_int(inf, &c[i].max_lvl);
        rs_read_int(inf, &c[i].factor);
        rs_read_int(inf, &c[i].offset);
        rs_read_int(inf, &c[i].range);
    }
    return(READSTAT);
}

int
rs_write_traps(FILE *savef, struct trap *trap,int count)
{
    int n;

	DBG(("Traps\n"));
    rs_write_int(savef, RSXR_TRAP);
    rs_write_int(savef, count);
    
    for(n=0; n<count; n++)
    {
        rs_write_char(savef, trap[n].tr_type);
        rs_write_char(savef, trap[n].tr_show);
        rs_write_coord(savef, &trap[n].tr_pos);
        rs_write_long(savef, trap[n].tr_flags);
    }

    return(WRITESTAT);
}

int
rs_read_traps(int inf, struct trap *trap, int count)
{
    int id = 0, value = 0, n = 0;

    if (rs_read_int(inf,&id) != 0)
    {
        if (id != RSXR_TRAP)
        {
            printf("Invalid id. %x != %x(RSXR_TRAP)\n",
                id,RSXR_TRAP);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else if (rs_read_int(inf,&value) != 0)
        {
            if (value != count)
            {
                printf("Incorrect number of traps in block. %d > %d.",
                    value,count);
                printf("Sorry, invalid save game format\n");
                format_error = TRUE;
            }
            else
            {
                for(n=0;n<value;n++)
                {   
                    rs_read_char(inf,&trap[n].tr_type);
                    rs_read_char(inf,&trap[n].tr_show);
                    rs_read_coord(inf,&trap[n].tr_pos);
                    rs_read_long(inf,&trap[n].tr_flags);
                }
            }
        }
        else
            format_error = TRUE;
    }
    
    return(READSTAT);
}

int
rs_write_monsters(FILE * savef, struct monster * m, int count)
{
    int n;
    
	DBG(("Monsters\n"));
    rs_write_int(savef, RSXR_MONSTERS);
    rs_write_int(savef, count);

    for(n=0;n<count;n++)
    {
        rs_write_string(savef, m[n].m_name);
        /*printf("Monster: %s/%d/%d\n",m[n].m_name,sizeof(m[n].m_name),strlen(m[n].m_name));*/
        rs_write_short(savef, m[n].m_carry);
        rs_write_boolean(savef, m[n].m_normal);
        rs_write_boolean(savef, m[n].m_wander);
        rs_write_char(savef, m[n].m_appear);
        rs_write_string(savef, m[n].m_intel);
        rs_write_longs(savef, m[n].m_flags, MAXFLAGS);
        rs_write_string(savef, m[n].m_typesum);
        rs_write_short(savef, m[n].m_numsum);
        rs_write_short(savef, m[n].m_add_exp);

        rs_write_mstats(savef, &m[n].m_stats);
    }
    
    return(WRITESTAT);
}

int
rs_read_monsters(int inf, struct monster *m, int count)
{
    int id = 0, value = 0, n = 0;
    
    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_MONSTERS)
        {
            printf("Invalid id. %x != %x(RSXR_MONSTERS)\n",
                id,RSXR_MONSTERS);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else if (rs_read_int(inf, &value) != 0)
        {
			if (value != count)
			{   printf("Incorrect number of monsters in block. %d != %d.",
                    value,count);
                printf("Sorry, invalid save game format\n");
                format_error = TRUE;

			}
			else for(n=0;n<value;n++)
            {
                rs_read_string(inf, m[n].m_name,sizeof(m[n].m_name));
                /*printf("Monster: %s\n",m[n].m_name);*/
                rs_read_short(inf, &m[n].m_carry);
				/*printf("    Carry: %d\n",m[n].m_carry); */
                rs_read_boolean(inf, &m[n].m_normal);
				/*printf("    Normal: %d\n",m[n].m_normal);*/
                rs_read_boolean(inf, &m[n].m_wander);
				/*printf("    Wander: %d\n",m[n].m_wander);*/
                rs_read_char(inf, &m[n].m_appear);
				/*printf("    Appears: %c\n",m[n].m_appear);*/
                rs_read_string(inf, m[n].m_intel,sizeof(m[n].m_intel));
				/*printf("    Intelligence: %s\n",m[n].m_intel);*/
                rs_read_longs(inf, m[n].m_flags, MAXFLAGS);
				/*printf("    Flags: %X\n",m[n].m_flags);*/
                rs_read_string(inf, m[n].m_typesum, sizeof(m[n].m_typesum));
				/*printf("    Summons: %s\n",m[n].m_typesum);*/
                rs_read_short(inf, &m[n].m_numsum);
				/*printf("    # Summons: %d\n",m[n].m_numsum);*/
                rs_read_short(inf, &m[n].m_add_exp);
				/*printf("    Experience: %d\n",m[n].m_add_exp);*/
                rs_read_mstats(inf, &m[n].m_stats);

             }
        }
        else
            format_error = TRUE;
    }
    
    return(READSTAT);
}

/*****************************************************************************/

int
rs_write_coord_list(FILE *savef, struct linked_list *l)
{
	DBG(("Coordinate List\n"));
    rs_write_int(savef, RSXR_COORDLIST);
    rs_write_int(savef, list_size(l));

    while (l != NULL) 
    {
        rs_write_coord(savef, (coord *) l->l_data);
        l = l->l_next;
    }
    
    return(WRITESTAT);
}

int
rs_read_coord_list(int inf, struct linked_list **list)
{
    int id;
    int i, cnt;
    struct linked_list *l = NULL, *previous = NULL, *head = NULL;

    if (rs_read_int(inf,&id) != 0)
    {
        if (id != RSXR_COORDLIST)
        {
            printf("Invalid id. %x != %x(RSXR_COORDLIST)\n",
                id,RSXR_COORDLIST);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
	else if (rs_read_int(inf,&cnt) != 0)
        {
            for (i = 0; i < cnt; i++)
            {
                l = new_item(sizeof(coord));
                l->l_prev = previous;
                if (previous != NULL)
                    previous->l_next = l;
                l->l_data = malloc(sizeof(coord));
                memset(l->l_data,0,sizeof(coord));
                rs_read_coord(inf,(coord *) l->l_data);
                if (previous == NULL)
                    head = l;
                previous = l;
            }
            
            if (l != NULL)
                l->l_next = NULL;
    
            *list = head;
        }
        else
            format_error = TRUE;
    }
    else
        format_error = TRUE;

    return(READSTAT);
}

int
rs_write_object_list(FILE *savef, struct linked_list *l)
{
	DBG(("Object List\n"));
    rs_write_int(savef, RSXR_OBJECTLIST);
    rs_write_int(savef, list_size(l));

    while (l != NULL) 
    {
        rs_write_object(savef, (struct object *) l->l_data);
        l = l->l_next;
    }
    
    return(WRITESTAT);
}

int
rs_read_object_list(int inf, struct linked_list **list)
{
    int id;
    int i, cnt;
    struct linked_list *l = NULL, *previous = NULL, *head = NULL;

    if (rs_read_int(inf,&id) != 0)
    {
        if (rs_read_int(inf,&cnt) != 0)
        {
            for (i = 0; i < cnt; i++) 
            {
                l = new_item(sizeof(struct object));
                memset(l->l_data,0,sizeof(struct object));
                l->l_prev = previous;
                if (previous != NULL)
                    previous->l_next = l;
                rs_read_object(inf,(struct object *) l->l_data);
                if (previous == NULL)
                    head = l;
                previous = l;
            }
            
            if (l != NULL)
                l->l_next = NULL;
    
            *list = head;
        }
        else
            format_error = TRUE;
    }
    else
        format_error = TRUE;


    return(READSTAT);
}

int
rs_write_thing(FILE *savef, struct thing *t)
{
	DBG(("Thing\n"));
	save_debug = FALSE;
    rs_write_int(savef, RSXR_THING);
    rs_write_boolean(savef,t->t_wasshot);
    rs_write_char(savef, t->t_type);
    rs_write_char(savef, t->t_disguise);
    rs_write_char(savef, t->t_oldch);
    rs_write_short(savef, t->t_ctype);
    rs_write_short(savef, t->t_index);
    rs_write_short(savef, t->t_no_move);
    rs_write_short(savef, t->t_quiet);
    rs_write_short(savef, t->t_movement);
    rs_write_short(savef, t->t_action);
    rs_write_short(savef, t->t_artifact);
    rs_write_short(savef, t->t_wand);
    rs_write_short(savef, t->t_summon);
    rs_write_short(savef, t->t_cast);
    rs_write_short(savef, t->t_breathe);
    rs_write_string(savef,t->t_name);
    rs_write_coord(savef, &t->t_doorgoal);

    if (t->t_dest == &hero)
    {
        rs_write_int(savef,1);
        rs_write_int(savef,-100);
    }
    else if (t->t_dest == NULL)
    {
        rs_write_int(savef,0);
        rs_write_int(savef,-100);
    }
	else
	{
		/* TODO:CRITICAL
		
		t_dest is a pointer to a t_pos field of a monster in
		one of our lists or is a pointer to o_pos field of an object
		in one of our lists. need to write out an id for the
		list and an index within that list.
		*/
	
		rs_write_coord(savef,t->t_dest);
	}

    rs_write_coord(savef, &t->t_pos);
    rs_write_coord(savef, &t->t_oldpos);
    rs_write_coord(savef, &t->t_newpos);
    rs_write_ulongs(savef, t->t_flags, 16);
	save_debug = TRUE;

	DBG(("Thing\n"));
	DBG(("    SaveID  : %X\n",RSXR_THING));
	DBG(("    Name    : %s\n",t->t_name));
	DBG(("    WasShot : %d\n",t->t_wasshot));
	DBG(("    Type    : %c(%d)\n",t->t_type,t->t_type));
	DBG(("    Disguise: %c(%d)\n",t->t_disguise,t->t_disguise));
	DBG(("    OldCh   : %c(%d)\n",t->t_oldch,t->t_oldch));
	DBG(("    CType   : %d\n",t->t_ctype));
	DBG(("    Index   : %d\n",t->t_index));
	DBG(("    NoMove  : %d\n",t->t_no_move));
	DBG(("    Quiet   : %d\n",t->t_quiet));
	DBG(("    Movement: %d\n",t->t_movement));
	DBG(("    Action  : %d\n",t->t_action));
	DBG(("    Artifact: %d\n",t->t_artifact));
	DBG(("    Wand    : %d\n",t->t_wand));
	DBG(("    Summon  : %d\n",t->t_summon));
	DBG(("    Cast    : %d\n",t->t_cast));
	DBG(("    Breathe : %d\n",t->t_breathe));
	DBG(("    DoorGoal: %d %d\n",t->t_doorgoal.x,t->t_doorgoal.y));
	if (t->t_dest)
	{
		DBG(("    Dest    : %d %d\n",t->t_dest->x,t->t_dest->y));
	}
	else
	{
		DBG(("    Dest    : None\n"));
	}
	DBG(("    Pos     : %d %d\n",t->t_pos.x,t->t_pos.y));
	DBG(("    OldPos  : %d %d\n",t->t_oldpos.x,t->t_oldpos.y));
	DBG(("    NewPos  : %d %d\n",t->t_newpos.x,t->t_newpos.y));
	DBG(("    Flags   : "));
	{ int i; for(i=0;i<16;i++) {DBG(("%d ",t->t_flags[i]));} DBG(("\n")); }
	save_debug = TRUE;


    rs_write_object_list(savef, t->t_pack);
    rs_write_object_list(savef, t->t_using);
    rs_write_stats(savef, &t->t_stats);
    rs_write_stats(savef, &t->maxstats);
    
    return(WRITESTAT);
}

int
rs_read_thing(int inf, struct thing *t)
{
    int id;
    static coord tdest;
    
    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_THING)
            format_error = TRUE;
        else
        {
            rs_read_boolean(inf,&t->t_wasshot);
            rs_read_char(inf, &t->t_type);
            rs_read_char(inf, &t->t_disguise);
            rs_read_char(inf, &t->t_oldch);
            rs_read_short(inf, &t->t_ctype);
            rs_read_short(inf, &t->t_index);
            rs_read_short(inf, &t->t_no_move);
            rs_read_short(inf, &t->t_quiet);
            rs_read_short(inf, &t->t_movement);
            rs_read_short(inf, &t->t_action);
            rs_read_short(inf, &t->t_artifact);
            rs_read_short(inf, &t->t_wand);
            rs_read_short(inf, &t->t_summon);
            rs_read_short(inf, &t->t_cast);
            rs_read_short(inf, &t->t_breathe);
            rs_read_string(inf,t->t_name,sizeof(t->t_name));
            rs_read_coord(inf,&t->t_doorgoal);

			t->t_dest = &tdest;
            rs_read_coord(inf,t->t_dest);

			if ((t->t_dest->x == 1) && (t->t_dest->y == -100))
				t->t_dest = &hero;
			else if ((t->t_dest->x == 0) && (t->t_dest->y == -100))
				t->t_dest = NULL;
			else
				t->t_dest = NULL;
				
            rs_read_coord(inf,&t->t_pos);
            rs_read_coord(inf,&t->t_oldpos);
            rs_read_coord(inf,&t->t_newpos);
            rs_read_ulongs(inf,t->t_flags,16);
            rs_read_object_list(inf,&t->t_pack);
            rs_read_object_list(inf,&t->t_using);
            rs_read_stats(inf,&t->t_stats);
            rs_read_stats(inf,&t->maxstats);
        }
    }
    else format_error = TRUE;
    
    return(READSTAT);
}

int
find_list_ptr(struct linked_list *l, void *ptr)
{
    int count = 0;

    while(l != NULL)
    {
        if (l->l_data == ptr)
            return(count);
            
        l = l->l_next;
        count++;
    }
    
    return(-1);
}


int
list_size(struct linked_list *l)
{
    int count = 0;
    
    while(l != NULL)
    {
        if (l->l_data == NULL)
            return(count);
            
        count++;
        
        l = l->l_next;
    }
    
    return(count);
}

int
rs_write_monster_list(FILE *savef, struct linked_list *l)
{
    int cnt = 0;
    
	DBG(("Monster List\n"));
    rs_write_int(savef, RSXR_MONSTERLIST);

    cnt = list_size(l);

    rs_write_int(savef, cnt);

    if (cnt < 1)
        return(WRITESTAT);

    while (l != NULL) {
        rs_write_thing(savef, (struct thing *)l->l_data);
        l = l->l_next;
    }
    
    return(WRITESTAT);
}

int
rs_read_monster_list(int inf, struct linked_list **list)
{
    int id;
    int i, cnt;
    struct linked_list *l = NULL, *previous = NULL, *head = NULL;

    if (rs_read_int(inf,&id) != 0)
    {
        if (id != RSXR_MONSTERLIST)
        {
            printf("Invalid id. %x != %x(RSXR_MONSTERLIST)\n",
                id,RSXR_MONSTERLIST);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }
        else if (rs_read_int(inf,&cnt) != 0)
        {
            for (i = 0; i < cnt; i++) 
            {
                l = new_item(sizeof(struct thing));
                l->l_prev = previous;
                if (previous != NULL)
                    previous->l_next = l;
                rs_read_thing(inf,(struct thing *)l->l_data);
                if (previous == NULL)
                    head = l;
                previous = l;
            }
        

            if (l != NULL)
                l->l_next = NULL;

            *list = head;
        }
    }
    else format_error = TRUE;
    
    return(READSTAT);
}

int
rs_write_magic_items(FILE *savef, struct magic_item *i, int count)
{
    int n;
    
	DBG(("Magic Items\n"));
    rs_write_int(savef, RSXR_MAGICITEMS);
    rs_write_int(savef, count);

    for(n=0;n<count;n++)
    {
        rs_write_string(savef,i[n].mi_name);
        rs_write_int(savef,i[n].mi_prob);
        rs_write_int(savef,i[n].mi_worth);
        rs_write_int(savef,i[n].mi_curse);
        rs_write_int(savef,i[n].mi_bless);
    }
    
    return(WRITESTAT);
}

int
rs_read_magic_items(int inf, struct magic_item *mi, int count)
{
    int id;
    int n;
    int value;

    if (rs_read_int(inf, &id) != 0)
    {
        if (id != RSXR_MAGICITEMS)
        {
            printf("Invalid id. %x != %x(RSXR_MAGICITEMS)\n",
                id,RSXR_MAGICITEMS);
            printf("Sorry, invalid save game format");
            format_error = TRUE;
        }   
        else if (rs_read_int(inf, &value) != 0)
        {
            if (value > count)
            {
                printf("Incorrect number of magic items in block. %d > %d.",
                    value,count);
                printf("Sorry, invalid save game format");
                format_error = TRUE;
            }
            else
            {
                for(n = 0; n < value; n++)
                {
                    rs_read_string(inf,mi[n].mi_name,sizeof(mi[n].mi_name));
                    rs_read_int(inf,&mi[n].mi_prob);
                    rs_read_int(inf,&mi[n].mi_worth);
                    rs_read_int(inf,&mi[n].mi_curse);
                    rs_read_int(inf,&mi[n].mi_bless);
                }
            }
        }
    }
    
    return(READSTAT);
}

int
rs_write_window(FILE *savef, WINDOW *win)
{
    int row,col,height,width;
    width = getmaxx(win);
    height = getmaxy(win);
	DBG(("Window\n"));
    rs_write_int(savef,height);
    rs_write_int(savef,width);
    
    for(row=0;row<height;row++)
        for(col=0;col<width;col++)
            rs_write_int(savef, mvwinch(win,row,col) );

    return(WRITESTAT);
}

int
rs_read_window(int inf, WINDOW *win)
{
    int row,col,maxlines,maxcols,value,width,height;
    
    width = getmaxx(win);
    height = getmaxy(win);

    rs_read_int(inf,&maxlines);
    rs_read_int(inf,&maxcols);
    if (maxlines > height)
       abort();
    if (maxcols > width)
       abort();
    
    for(row=0;row<maxlines;row++)
        for(col=0;col<maxcols;col++)
        {
            rs_read_int(inf, &value);
            mvwaddch(win,row,col,value);
        }

    return(READSTAT);
}

int
rs_save_file(int savefd)
{
    int i, weapon, armor, ring, misc, room = -1;
    FILE *savef;

    savef = fdopen(savefd,"w");
    rs_write_traps(savef, traps, MAXTRAPS);             /* rogue.c          */

    rs_write_thing(savef, &player);                     /* rogue.c          */
    armor = find_list_ptr(player.t_pack, cur_armor);
    rs_write_int(savef, armor);                         /* rogue.c          */

    for (i = 0; i < NUM_FINGERS; i++)
    {
        ring = find_list_ptr(player.t_pack, cur_ring[i]);
        rs_write_int(savef, ring);                      /* rogue.c          */
    }

    for (i = 0; i < NUM_MM; i++)
    {
        misc = find_list_ptr(player.t_pack, cur_misc[i]);
        rs_write_int(savef, misc);                      /* rogue.c          */
    }

    for (i=0; i<MAXRELIC; i++)
       rs_write_int(savef,cur_relic[i]);                /* rogue.c          */

    rs_write_object_list(savef, lvl_obj);               /* rogue.c          */
    rs_write_monster_list(savef, mlist);                /* rogue.c          */
    rs_write_monster_list(savef, tlist);                /* rogue.c          */

    if (monst_dead != NULL)
        i = find_list_ptr(mlist, monst_dead);
    else
        i = -1;

    rs_write_int(savef,i);                              /* rogue.c          */

    rs_write_rooms(savef, rooms, MAXROOMS);             /* rogue.c          */

    for (i = 0; i < MAXROOMS; i++)
        if (&rooms[i] == oldrp)
            room = i;
    rs_write_int(savef, room);                          /* rogue.c          */

    weapon = find_list_ptr(player.t_pack, cur_weapon);
    rs_write_int(savef, weapon);                        /* rogue..c         */
    rs_write_int(savef,char_type);
    rs_write_int(savef,foodlev);
    rs_write_int(savef,ntraps);
    rs_write_int(savef,trader);
    rs_write_int(savef,curprice);
    rs_write_int(savef,seed);
    rs_write_int(savef,max_level);
    rs_write_int(savef,cur_max);
    rs_write_int(savef,prev_max);
    rs_write_int(savef,move_free);
    rs_write_int(savef,mpos);
    rs_write_int(savef,level);
    rs_write_long(savef,purse);
    rs_write_int(savef,inpack);
    rs_write_int(savef,total);
    rs_write_int(savef,no_food);
    rs_write_int(savef,foods_this_level);
    rs_write_int(savef,count);
    rs_write_int(savef,food_left);
    rs_write_int(savef,group);
    rs_write_int(savef,hungry_state);
    rs_write_int(savef,infest_dam);
    rs_write_int(savef,lost_str);
    rs_write_int(savef,lastscore);
    rs_write_int(savef,hold_count);
    rs_write_int(savef,trap_tries);
    rs_write_int(savef,chant_time);
    rs_write_int(savef,pray_time);
    rs_write_int(savef,spell_power);
    rs_write_long(savef,turns);
    rs_write_int(savef,quest_item);
    rs_write_int(savef,cols);
    rs_write_int(savef,lines);
    rs_write_int(savef,nfloors);
    rs_write(savef,curpurch,LINELEN);
    rs_write_char(savef,PLAYER);
    rs_write_char(savef,take);
    rs_write_int(savef,1234);/*checkpoint*/
    rs_write(savef,prbuf,LINELEN*2);
    rs_write_int(savef,1234);/*checkpoint*/
    rs_write_char(savef,runch);
    rs_write_int(savef,1234);/*checkpoint*/
    rs_write_strings(savef, s_names,  MAXSCROLLS);
    rs_write_strings(savef, p_colors, MAXPOTIONS);
    rs_write_strings(savef, r_stones, MAXRINGS);
    rs_write_strings(savef, ws_made,  MAXSTICKS);
    rs_write(savef,whoami,LINELEN);
    rs_write_strings(savef, s_guess,  MAXSCROLLS);
    rs_write_strings(savef, p_guess,  MAXPOTIONS);
    rs_write_strings(savef, r_guess,  MAXRINGS);
    rs_write_strings(savef, ws_guess, MAXSTICKS);
    rs_write_strings(savef, m_guess, MAXMM);
    rs_write_strings(savef, ws_type,  MAXSTICKS);
    rs_write_window(savef, cw);
    rs_write_window(savef, hw);
    rs_write_window(savef, mw);
    rs_write_window(savef, msgw);
    rs_write_window(savef, stdscr);
    rs_write_boolean(savef,pool_teleport);
    rs_write_boolean(savef,inwhgt);
    rs_write_boolean(savef,after);
    rs_write_boolean(savef,waswizard);
    rs_write_booleans(savef,s_know,MAXSCROLLS);             /* rogue.h          */
    rs_write_booleans(savef,p_know,MAXPOTIONS);             /* rogue.h          */
    rs_write_booleans(savef,r_know,MAXRINGS);               /* rogue.h          */
    rs_write_booleans(savef,ws_know,MAXSTICKS);             /* rogue.h          */
    rs_write_booleans(savef,m_know,MAXMM);             /* rogue.h          */
    rs_write_boolean(savef, playing);                       /* rogue.h/init.c   */
    rs_write_boolean(savef, running);                       /* rogue.h/init.c   */
    rs_write_boolean(savef, wizard);                        /* rogue.h/init.c   */
    rs_write_boolean(savef, notify);                        /* rogue.h/init.c   */
    rs_write_boolean(savef, fight_flush);                   /* rogue.h/init.c   */
    rs_write_boolean(savef, terse);                         /* rogue.h/init.c   */
    rs_write_boolean(savef, auto_pickup);                     /* rogue.h/init.c   */
    rs_write_boolean(savef, def_attr);                     /* rogue.h/init.c   */
    rs_write_boolean(savef, menu_overlay);                     /* rogue.h/init.c   */
    rs_write_boolean(savef, door_stop);                     /* rogue.h/init.c   */
    rs_write_boolean(savef, jump);                          /* rogue.h/init.c   */
    rs_write_boolean(savef, slow_invent);                   /* rogue.h/init.c   */
    rs_write_boolean(savef, firstmove);                     /* rogue.h/init.c   */
    rs_write_boolean(savef, askme);                         /* rogue.h/init.c   */
    rs_write_boolean(savef, in_shell);                      /* rogue.h/init.c   */
    rs_write_boolean(savef, daytime);                      /* rogue.h/init.c   */
    rs_write_boolean(savef, funfont);                      /* rogue.h/init.c   */
    rs_write_levtype(savef,levtype); 
	rs_write_character_types(savef,char_class,NUM_CHARTYPES);
    for(i=0;i<9;i++)
        rs_write_coord(savef,&grid[i]);
    rs_write_death_types(savef,deaths,DEATHNUM);
    rs_write_init_weps(savef,weaps,MAXWEAPONS);
    rs_write_init_armor(savef,armors,MAXARMORS);
    rs_write_magic_items(savef, things,   NUMTHINGS);   /* rogue.h/init.c   */
    rs_write_magic_items(savef, s_magic,  MAXSCROLLS);  /* rogue.h/init.c   */
    rs_write_magic_items(savef, p_magic,  MAXPOTIONS);  /* rogue.h/init.c   */
    rs_write_magic_items(savef, r_magic,  MAXRINGS);    /* rogue.h/init.c   */
    rs_write_magic_items(savef, ws_magic, MAXSTICKS);   /* rogue.h/init.c   */   
    rs_write_magic_items(savef, m_magic, MAXMM);   /* rogue.h/init.c   */
    rs_write_magic_items(savef, rel_magic, MAXRELIC);   /* rogue.h/init.c   */
    rs_write_magic_items(savef, foods, MAXFOODS);   /* rogue.h/init.c   */
    rs_write_spells(savef,magic_spells,MAXSPELLS);
    rs_write_spells(savef,cleric_spells,MAXPRAYERS);
    rs_write_spells(savef,druid_spells,MAXCHANTS);
    rs_write_spells(savef,quill_scrolls,MAXQUILL);

    rs_write_int(savef,mf_count);                           /* actions.c        */
    rs_write_int(savef,mf_jmpcnt);                          /* actions.c        */
    rs_write_daemons(savef, d_list, MAXDAEMONS);            /* daemon.c         */
    rs_write_daemons(savef, f_list, MAXFUSES);              /* daemon.c         */
    rs_write_int(savef,demoncnt);                           /* daemon.c         */
    rs_write_int(savef,fusecnt);                            /* daemon.c         */
    rs_write_int(savef,killed_chance);                      /* fight.c          */
    rs_write_scrolls(savef);                                /* init.c           */
    rs_write_potions(savef);                                /* init.c           */
    rs_write_rings(savef);                                  /* init.c           */
    rs_write_sticks(savef);                                 /* init.c           */
    rs_write_monsters(savef,monsters,NUMMONST+1);           /* mons_def.c       */
    rs_write_coord(savef,&move_nh);                         /* move.c           */
    return(WRITESTAT);
}

int
rs_restore_file(int inf)
{
    int weapon, armor, ring, misc, room = -1,i,checkpoint;

    rs_read_traps(inf, traps, MAXTRAPS);

    rs_read_thing(inf, &player);                        /* rogue.h          */

    rs_read_int(inf, &armor);                           /* rogue.h          */
    cur_armor = get_list_item(player.t_pack,armor);

    for(i = 0; i < NUM_FINGERS; i++)
    {
        rs_read_int(inf,&ring);
        cur_ring[i] = get_list_item(player.t_pack,ring);
    }

    for(i = 0; i < NUM_MM; i++)
    {
        rs_read_int(inf,&misc);
        cur_misc[i] = get_list_item(player.t_pack,misc);
    }

    for(i=0;i<MAXRELIC;i++)
        rs_read_int(inf,&cur_relic[i]);

    rs_read_object_list(inf, &lvl_obj);                 /* rogue.h/init.c   */
    rs_read_monster_list(inf, &mlist);                  /* rogue.h/init.c   */
    rs_read_monster_list(inf, &tlist);                  /* rogue.h/init.c   */

    rs_read_int(inf,&i);
    if (i == -1)
        monst_dead = NULL;
    else
        monst_dead = get_list_item(mlist,i);

    rs_read_rooms(inf, rooms, MAXROOMS);
    rs_read_int(inf, &room);
    
    oldrp = &rooms[room];

    rs_read_int(inf,&weapon);   
	cur_weapon = get_list_item(player.t_pack,weapon);

    rs_read_int(inf,&char_type);
    rs_read_int(inf,&foodlev);
    rs_read_int(inf,&ntraps);
    rs_read_int(inf,&trader);
    rs_read_int(inf,&curprice);
    rs_read_int(inf,&seed);
    rs_read_int(inf,&max_level);
    rs_read_int(inf,&cur_max);
    rs_read_int(inf,&prev_max);
    rs_read_int(inf,&move_free);
    rs_read_int(inf,&mpos);
    rs_read_int(inf,&level);
    rs_read_long(inf,&purse);
    rs_read_int(inf,&inpack);
    rs_read_int(inf,&total);
    rs_read_int(inf,&no_food);
    rs_read_int(inf,&foods_this_level);
    rs_read_int(inf,&count);
    rs_read_int(inf,&food_left);
    rs_read_int(inf,&group);
    rs_read_int(inf,&hungry_state);
    rs_read_int(inf,&infest_dam);
    rs_read_int(inf,&lost_str);
    rs_read_int(inf,&lastscore);
    rs_read_int(inf,&hold_count);
    rs_read_int(inf,&trap_tries);
    rs_read_int(inf,&chant_time);
    rs_read_int(inf,&pray_time);
    rs_read_int(inf,&spell_power);
    rs_read_long(inf,&turns);
    rs_read_int(inf,&quest_item);
    rs_read_int(inf,&cols);
    rs_read_int(inf,&lines);
    rs_read_int(inf,&nfloors);
    rs_read(inf,curpurch,LINELEN);
    rs_read_char(inf,&PLAYER);
    rs_read_char(inf,&take);
    rs_read_int(inf,&checkpoint);
    if (checkpoint != 1234){printf("Checkpoint failed");abort();}
    rs_read(inf,prbuf,LINELEN*2);
    rs_read_int(inf,&checkpoint);
    if (checkpoint != 1234){printf("Checkpoint failed");abort();}
    rs_read_char(inf,&runch);
    rs_read_int(inf,&checkpoint);
    if (checkpoint != 1234){printf("Checkpoint failed");abort();}
    rs_read_new_strings(inf, s_names,  MAXSCROLLS);
    rs_read_new_strings(inf, p_colors, MAXPOTIONS);
    rs_read_new_strings(inf, r_stones, MAXRINGS);
    rs_read_new_strings(inf, ws_made,  MAXSTICKS);
    rs_read(inf,whoami,LINELEN);
    rs_read_new_strings(inf, s_guess,  MAXSCROLLS);
    rs_read_new_strings(inf, p_guess,  MAXPOTIONS);
    rs_read_new_strings(inf, r_guess,  MAXRINGS);
    rs_read_new_strings(inf, ws_guess, MAXSTICKS);
    rs_read_new_strings(inf, m_guess, MAXMM);
    rs_read_new_strings(inf, ws_type,  MAXSTICKS);
    rs_read_window(inf, cw);
    rs_read_window(inf, hw);
    rs_read_window(inf, mw);
    rs_read_window(inf, msgw);
    rs_read_window(inf, stdscr);
    rs_read_boolean(inf,&pool_teleport);
    rs_read_boolean(inf,&inwhgt);
    rs_read_boolean(inf,&after);
    rs_read_boolean(inf,&waswizard);
    rs_read_booleans(inf,s_know,MAXSCROLLS);             /* rogue.h          */
    rs_read_booleans(inf,p_know,MAXPOTIONS);             /* rogue.h          */
    rs_read_booleans(inf,r_know,MAXRINGS);               /* rogue.h          */
    rs_read_booleans(inf,ws_know,MAXSTICKS);             /* rogue.h          */
    rs_read_booleans(inf,m_know,MAXMM);             /* rogue.h          */
    rs_read_boolean(inf, &playing);                       /* rogue.h/init.c   */
    rs_read_boolean(inf, &running);                       /* rogue.h/init.c   */
    rs_read_boolean(inf, &wizard);                        /* rogue.h/init.c   */
    rs_read_boolean(inf, &notify);                        /* rogue.h/init.c   */
    rs_read_boolean(inf, &fight_flush);                   /* rogue.h/init.c   */
    rs_read_boolean(inf, &terse);                         /* rogue.h/init.c   */
    rs_read_boolean(inf, &auto_pickup);                     /* rogue.h/init.c   */
    rs_read_boolean(inf, &def_attr);                     /* rogue.h/init.c   */
    rs_read_boolean(inf, &menu_overlay);                     /* rogue.h/init.c   */
    rs_read_boolean(inf, &door_stop);                     /* rogue.h/init.c   */
    rs_read_boolean(inf, &jump);                          /* rogue.h/init.c   */
    rs_read_boolean(inf, &slow_invent);                   /* rogue.h/init.c   */
    rs_read_boolean(inf, &firstmove);                     /* rogue.h/init.c   */
    rs_read_boolean(inf, &askme);                         /* rogue.h/init.c   */
    rs_read_boolean(inf, &in_shell);                      /* rogue.h/init.c   */
    rs_read_boolean(inf, &daytime);                      /* rogue.h/init.c   */
    rs_read_boolean(inf, &funfont);                      /* rogue.h/init.c   */
    rs_read_levtype(inf,&levtype);
    rs_read_character_types(inf,char_class,NUM_CHARTYPES);
    for(i=0;i<9;i++)
        rs_read_coord(inf,&grid[i]);
    rs_read_death_types(inf,deaths,DEATHNUM);
    rs_read_init_weps(inf,weaps,MAXWEAPONS);
    rs_read_init_armor(inf,armors,MAXARMORS);
    rs_read_magic_items(inf, things,NUMTHINGS);         /* rogue.h/init.c   */
    rs_read_magic_items(inf, s_magic,MAXSCROLLS);       /* rogue.h/init.c   */
    rs_read_magic_items(inf, p_magic,MAXPOTIONS);       /* rogue.h/init.c   */
    rs_read_magic_items(inf, r_magic,MAXRINGS);         /* rogue.h/init.c   */
    rs_read_magic_items(inf, ws_magic,MAXSTICKS);       /* rogue.h/init.c   */
    rs_read_magic_items(inf, m_magic,MAXMM);       /* rogue.h/init.c   */
    rs_read_magic_items(inf, rel_magic,MAXRELIC);       /* rogue.h/init.c   */
    rs_read_magic_items(inf, foods,MAXFOODS);       /* rogue.h/init.c   */
    rs_read_spells(inf,magic_spells,MAXSPELLS);
    rs_read_spells(inf,cleric_spells,MAXPRAYERS);
    rs_read_spells(inf,druid_spells,MAXCHANTS);
    rs_read_spells(inf,quill_scrolls,MAXQUILL);

    rs_read_int(inf,&mf_count);                             /* actions.c        */
    rs_read_int(inf,&mf_jmpcnt);                            /* actions.c        */
    rs_read_daemons(inf, d_list, MAXDAEMONS);               /* daemon.c         */
    rs_read_daemons(inf, f_list, MAXFUSES);                 /* daemon.c         */
    rs_read_int(inf,&demoncnt);                             /* daemon.c         */
    rs_read_int(inf,&fusecnt);                              /* daemon.c         */
    rs_read_int(inf,&killed_chance);                        /* fight.c          */
    rs_read_scrolls(inf);                                   /* init.c           */
    rs_read_potions(inf);                                   /* init.c           */
    rs_read_rings(inf);                                     /* init.c           */
    rs_read_sticks(inf);                                    /* init.c           */


    rs_read_monsters(inf,monsters,NUMMONST+1);              /* mons_def.c       */
    rs_read_coord(inf,&move_nh);                            /* move.c           */

    if (READSTAT)
        printf("Game restored.\n");
    else
        printf("Game restoration failed\n");

    return(READSTAT);
}
