/*
 * save game and restore routines
 */

#include <curses.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "rogue.h"
#include "mach_dep.h"
#ifdef PC7300
#include <sys/window.h>
extern struct uwdata wdata;
#endif

#if u370 || uts || i386
#define ENCREAD(b,n,fd) read(fd,b,n)
#define ENCWRITE(b,n,fd) write(fd,b,n)
#endif
#ifndef ENCREAD
#define ENCREAD encread
#define ENCWRITE encwrite
#endif

typedef struct stat STAT;

extern char version[], *strerror();
extern const unsigned char encstr[];
/* extern bool _endwin; */

extern char __executable_start[], _end[];
extern char rg_bss[], *const rg_edata, *const rg_ebss;
extern int playit(), rg_restored();

long save_stamp[2];
chtype save_std[MAXLINES][MAXCOLS];
chtype save_cw[MAXLINES][MAXCOLS];
chtype save_mw[MAXLINES][MAXCOLS];

#ifdef __SANITIZE_ADDRESS__
#define NOCHECK	__attribute__((no_sanitize_address))
#define read	rg_read
#else
#define NOCHECK
#endif

static STAT sbuf;

save_ok()
{
    return ((char *) &food_left >= version && (char *) &food_left < rg_edata
	&& (char *) &sbuf >= rg_bss && (char *) (&sbuf + 1) <= rg_ebss);
}

bool
save_game()
{
    register int savefd;
    register int c;
    char buf[LINELEN];

    if (!save_ok())
    {
        msg("this xrogue cannot save a game");
        return FALSE;
    }
    /*
     * get file name
     */
    mpos = 0;
    if (file_name[0] != '\0')
    {
        msg("Save file (%s)? ", file_name);
        do
        {
            c = wgetch(cw);
            if (c == ESC) return(0);
        } while (c != 'n' && c != 'N' && c != 'y' && c != 'Y');
        mpos = 0;
        if (c == 'y' || c == 'Y')
        {
            msg("File name: %s", file_name);
            goto gotfile;
        }
    }

    do
    {
        msg("File name: ");
        mpos = 0;
        buf[0] = '\0';
        if (get_str(buf, msgw) == QUIT)
        {
            msg("");
            return FALSE;
        }
        strcpy(file_name, buf);
gotfile:
        if (stat(file_name, &sbuf) >= 0 && (sbuf.st_mode & S_IFMT) == S_IFREG)
            unlink(file_name);
        if ((savefd = open(file_name, O_WRONLY|O_CREAT|O_EXCL, 0400)) < 0)
            msg(strerror(errno));    /* fake perror() */
    } while (savefd < 0);

    /*
     * write out encrpyted file (after a stat)
     */
    if (save_file(savefd) == FALSE) {
        msg("Cannot create save file.");
        unlink(file_name);
        return(FALSE);
    }
    else return(TRUE);
}

/*
 * automatically save a file.  This is used if a HUP signal is recieved
 */

void
auto_save()
{
    register int savefd;
    register int i;

    for (i = 0; i < NSIG; i++)
        signal(i, SIG_IGN);
    if (save_ok()               &&
        file_name[0] != '\0'    && 
        pstats.s_hpt > 0)
    {
        if (stat(file_name, &sbuf) >= 0 && (sbuf.st_mode & S_IFMT) == S_IFREG)
            unlink(file_name);
        savefd = open(file_name, O_WRONLY|O_CREAT|O_EXCL, 0400);
        if (savefd >= 0)
            save_file(savefd);
    }
    endwin();
#ifdef PC7300
    endhardwin();
#endif
    exit(1);
}

/*
 * write the saved game on the file
 */

bool
save_file(savefd)
register int savefd;
{
    register unsigned long num_to_write, num_written;
    register int y, x;

    for (y = 0; y < lines && y < MAXLINES; y++)
        for (x = 0; x < cols && x < MAXCOLS; x++)
        {
            save_std[y][x] = mvwinch(stdscr, y, x);
            save_cw[y][x] = mvwinch(cw, y, x);
            save_mw[y][x] = mvwinch(mw, y, x);
        }
    wmove(cw, lines-1, 0);
    draw(cw);
    lseek(savefd, 0L, 0);
    fstat(savefd, &sbuf);
    save_stamp[0] = (long) playit - (long) version;
    save_stamp[1] = (long) rg_restored - (long) __executable_start;
    num_to_write = (rg_edata - version) + (rg_ebss - rg_bss);
    num_written = ENCWRITE(version, rg_edata - version, savefd);
    if (num_written == rg_edata - version)
        num_written += ENCWRITE(rg_bss, rg_ebss - rg_bss, savefd);
    close(savefd);
    if (num_to_write == num_written) return(TRUE);
    else return(FALSE);
}

restore(file, envp)
register char *file;
char **envp;
{
    register int inf;
    extern char **environ;
    char buf[LINELEN], name[MAXPATH];
    STAT sbuf2;
    int oldcol, oldline;        /* Old number of columns and lines */
    int y, x;

    if (strcmp(file, "-r") == 0)
        file = strcpy(name, file_name);
    if ((inf = open(file, O_RDONLY)) < 0
        || fstat(inf, &sbuf2) < 0)
    {
        perror(file);
        return FALSE;
    }
    if (sbuf2.st_uid != getuid())
    {
        printf("Sorry, saved game is not yours.\n");
        return FALSE;
    }
    if ((sbuf2.st_mode & 022) != 0)
    {
        printf("Sorry, saved game can be written by others.\n");
        return FALSE;
    }

    fflush(stdout);
    lseek(inf, 0L, 0);
    ENCREAD(buf, (long)(strlen(version) + 1), inf); /* check which version */
    if (strcmp(buf, version) != 0)
    {
        printf("Sorry, saved game is out of date.\n");
        return FALSE;
    }

    fflush(stdout);
    if (sbuf2.st_size != (rg_edata - version) + (rg_ebss - rg_bss))
    {
        printf("Sorry, saved game is out of date.\n");
        return FALSE;
    }

    lseek(inf, 0L, 0);
    if (ENCREAD(version, (long) (rg_edata - version), inf)
            != rg_edata - version
        || ENCREAD(rg_bss, (long) (rg_ebss - rg_bss), inf)
            != rg_ebss - rg_bss)
    {
        printf("Sorry, saved game is damaged.\n");
        return FALSE;
    }
    if (save_stamp[0] != (long) playit - (long) version
        || save_stamp[1] != (long) rg_restored - (long) __executable_start)
    {
        printf("Sorry, saved game is out of date.\n");
        return FALSE;
    }
    rg_rebase(__executable_start, _end, version, rg_edata, rg_bss, rg_ebss);
    if (!rg_restored())
    {
        printf("Sorry, saved game is damaged.\n");
        return FALSE;
    }

    /*
     * Get the lines and columns from the previous game
     */
    oldcol = cols;
    oldline = lines;

    /*
     * we do not close the file so that we will have a hold of the
     * inode for as long as possible
     */
    if (!wizard) {
        if (sbuf2.st_ino != sbuf.st_ino) {
            printf("Sorry, saved game is not in the same file.\n");
            return FALSE;
        }
#ifndef NOCHECKACCESS
        else if (sbuf2.st_ctime - sbuf.st_ctime > FUDGE_TIME) {
            printf("Sorry, file has been touched.\n");
            return FALSE;
        }
#endif
    }

    /*
     * defeat multiple restarting from the same place
     */
    if (!wizard) {
        if (sbuf2.st_nlink != 1) {
            printf("Cannot restore from a linked file\n");
            return FALSE;
        }
    }

    /*
     * Set the new terminal and make sure we aren't going to a smaller screen.
     */
    initscr();
    if (COLS < oldcol || LINES < oldline) {
        endwin();
        printf("Cannot restart the game on a smaller screen.\n");
        return FALSE;
    }

    if (!wizard && unlink(file) < 0) {
        endwin();
        printf("Cannot unlink file\n");
        return FALSE;
    }

    cw = newwin(lines, cols, 0, 0);
    mw = newwin(lines, cols, 0, 0);
    hw = newwin(lines, cols, 0, 0);
    msgw = newwin(4, cols, 0, 0);
    if (cw == NULL || hw == NULL || mw == NULL || msgw == NULL) {
        endwin();
        fprintf(stderr, "Can't get enough memory\n");
        return FALSE;
    }
    keypad(cw, TRUE);
    keypad(hw, TRUE);
    for (y = 0; y < lines && y < MAXLINES; y++)
        for (x = 0; x < cols && x < MAXCOLS; x++)
        {
            mvwaddch(stdscr, y, x, save_std[y][x]);
            mvwaddch(cw, y, x, save_cw[y][x]);
            mvwaddch(mw, y, x, save_mw[y][x]);
        }

    mpos = 0;
    environ = envp;
    strncpy(file_name, file, MAXPATH - 1);
    file_name[MAXPATH - 1] = '\0';
    setup();
    clearok(curscr, TRUE);
    touchwin(cw);
    msg("Welcome back!  --More-- ");
    wait_for(' ');
    playit();
    /*NOTREACHED*/
}

#define ENCWBSIZ        1024

/*
 * perform an encrypted write
 */

NOCHECK long
encwrite(start, size, outf)
register char *start;
register unsigned long size;
register int outf;
{
    register const unsigned char *ep;
    register int i = 0;
    unsigned long num_written = 0;
    char buf[ENCWBSIZ];

    ep = encstr;

    while (size--)
    {
        buf[i++] = *start++ ^ *ep++;
        if (*ep == '\0')
           ep = encstr;

        if (i == ENCWBSIZ || size == 0) {
            if (write(outf, buf, (unsigned)i) < i) 
                 return(num_written);
            else {
                num_written += i;
                i = 0;
            }
        }
    }
    return(num_written);
}

#define ENCRBSIZ        (2^15)

/*
 * perform an encrypted read
 */

NOCHECK long
encread(start, size, inf)
register char *start;
register unsigned long size;
int inf;
{
    register const unsigned char *ep;
    register int rd_siz;
    register long total_read;

    total_read = 0;
    while (total_read < size) {
        rd_siz = size-total_read > ENCRBSIZ ? ENCRBSIZ : size-total_read;
        if((rd_siz=read(inf,&start[total_read],rd_siz))==-1 || rd_siz==0)
                break;
        total_read += rd_siz;
    }
    ep = encstr;

    size = total_read;
    while (size--)
    {
        *start++ ^= *ep++;
        if (*ep == '\0')
            ep = encstr;
    }
    return total_read;
}

