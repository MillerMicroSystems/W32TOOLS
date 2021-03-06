/* ----------------------------------------------------------------------- *\
|
|				     POPEN
|
|		    Copyright (c) 1985, 1990 all rights reserved
|				Brian W Johnson
|				   26-May-90
|				   17-Dec-94
|				   08-Oct-10 (corrections to looping)
|
|	    int
|	fd = popen (path, mode, perm);	Build the path and open the file
|	    char  *path;		Path/filename
|	    int    mode;		Open mode
|	    int    perm;		Create permission (O_CREAT mode)
|
|	    int    fd;			File number or error code
|
|	popen() is equivalent to open() except it will build the path also.
|
|	path is the path/filename of a file to be opened for writing.
|	popen() attempts to open the file.  If that fails, then the path
|	is checked and created as necessary.  One last attempt is made
|	to open the file.
|
|	popen() returns the open file descriptor, or (-1) for failure.
|
\* ----------------------------------------------------------------------- */

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <fcntl.h>
#include  <direct.h>
#include  <io.h>

#include  "fwild.h"

/* ----------------------------------------------------------------------- */
    int
popen (				/* Build path and open() a file */
    char  *s,			/* Pointer to the path/filename */
    int    mode,		/* Open mode */
    int    perm)		/* Create permission */

    {
    int    finished = 0;	/* Finished flag */
    int    fd;			/* File number or error code */
    int    SkipCount;		/* Ignore counter for the do loop */
    char  *p;			/* Pointer into the temporary string */
    char   temp [1024];		/* Temporary path/filename string */


				/* Allow for possible UNC path */
    SkipCount = (strncmp("\\\\", s, 2) == 0) ? (2) : (0);

    for (;;)
	{
	if (((fd = open(s, mode, perm)) >= 0)  ||  finished++)
	    break;		// File successfully opened

	if ((mode & (O_CREAT | O_WRONLY | O_RDWR)) == 0)
	    break;		// No intent to write, just report error

	p = &temp[0];		// Attempt to build the path
	do  {
	    strcpy(&temp[0], s);
	    if ((p = strpbrk((p + 1), "/\\"))
	    && (--SkipCount < 0))	/* Skip over the UNC part of the path */
		{
//printf("\npopen: (%d) \"%s\"  \"%d\"\n", count, &temp[0], (p - &temp[0]));
		*p = '\0';
		if (( ! fnchkdir(&temp[0]))
		&& ((fd = mkdir(&temp[0])) != 0))
		    break;	// Path building complete
		}
	    } while (p);

	}
    return  (fd);
    }

/* ----------------------------------------------------------------------- */
