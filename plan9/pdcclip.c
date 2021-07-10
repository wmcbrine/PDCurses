#include <stdlib.h>
#include <string.h>
#include <curspriv.h>
#include "pdcplan9.h"


static char *clip = NULL;

int PDC_getclipboard(char **sp, long *np)
{
	long n;

	PDC_LOG(("PDC_getclipboard() - called\n"));
	if (!clip)
		return PDC_CLIP_EMPTY;

	n = strlen(clip);
	if ((*sp = malloc(n + 1)) == NULL)
		return PDC_CLIP_MEMORY_ERROR;

	memmove(*sp, clip, n);
	*np = n;

	return PDC_CLIP_SUCCESS;
}


int PDC_setclipboard(const char *s, long n)
{
	PDC_LOG(("PDC_setclipboard() - called\n"));
	if (clip)
	{
		free(clip);
		clip = NULL;
	}
	if (s)
	{
		if ((clip = malloc(n + 1)) == NULL)
			return PDC_CLIP_MEMORY_ERROR;
		memmove(clip, s, n);
		clip[n] = 0;
	}

	return PDC_CLIP_SUCCESS;
}


int PDC_freeclipboard(char *s)
{
	PDC_LOG(("PDC_freeclipboard() - called\n"));
	free(s);

	return PDC_CLIP_SUCCESS;
}


int PDC_clearclipboard(void)
{
	PDC_LOG(("PDC_clearclipboard() - called\n"));
	if (clip)
	{
		free(clip);
		clip = NULL;
	}

	return PDC_CLIP_SUCCESS;
}
