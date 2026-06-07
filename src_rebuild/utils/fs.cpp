#include "fs.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32

#include <direct.h>
#include <wtypes.h>

#define HOME_ENV "USERPROFILE"

#elif defined(__SWITCH__)

#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

#elif defined (__unix__)

#include <sys/stat.h>
#include <glob.h>		// glob(), globfree()
#include <malloc.h>

#endif

void FS_FixPathSlashes(char* pathbuff)
{
#ifdef _WIN32
	const char oldSlash = '/';
	const char newSlash = '\\';
#else
	const char oldSlash = '\\';
	const char newSlash = '/';
#endif

	while (*pathbuff)
	{
		if (*pathbuff == oldSlash)
			*pathbuff = newSlash;
		pathbuff++;
	}
}

struct FS_FINDDATA
{
    char* wildcard;

#ifdef _WIN32
    WIN32_FIND_DATAA	wfd;
    HANDLE				fileHandle;
#elif defined(__SWITCH__)
	DIR*				dir;
	const char*			dirPath;
	const char*			pattern;
	const char*			currentName;
#else
    glob_t				gl;
    int					index;
    int					pathlen;
#endif // _WIN32
};

char* strreplace(char** str, char* dst, char* replace)
{
	char* start, * tmp;
	int n;

	if ((start = strstr(*str, dst)) == 0) 
		return(0);

	n = (start - *str) + strlen(start + strlen(dst)) + strlen(replace) + 1;

	tmp = (char*)malloc(n);

	// left part
	strncpy(tmp, *str, start - *str);

	// replace (mid) part
	strcpy(tmp + (start - *str), replace);

	// right part
	strcat(tmp, start + strlen(dst));

	free(*str);

	*str = tmp;

	return tmp;
}

#if defined(__SWITCH__)
static const char* FS_FindNextMatch(FS_FINDDATA* findData)
{
	struct dirent* ent;

	while ((ent = readdir(findData->dir)) != NULL)
	{
		const char* name = ent->d_name;

		if (!strcmp(name, ".") || !strcmp(name, ".."))
			continue;

		if (fnmatch(findData->pattern, name, 0) != 0)
			continue;

		findData->currentName = name;
		return name;
	}

	return nullptr;
}
#endif

// opens directory for search props
const char* FS_FindFirst(const char* wildcard, FS_FINDDATA** findData)
{
	assert(findData != nullptr);

	if (findData == nullptr)
		return nullptr;

	FS_FINDDATA* newFind = new FS_FINDDATA;
	*findData = newFind;

	newFind->wildcard = (char*)malloc(strlen(wildcard) + 1);
	strcpy(newFind->wildcard, wildcard);
	FS_FixPathSlashes(newFind->wildcard);

#ifndef _WIN32
	strreplace(&newFind->wildcard, "*.*", "*");
#endif

#ifdef _WIN32
	newFind->fileHandle = ::FindFirstFileA(newFind->wildcard, &newFind->wfd);

	if (newFind->fileHandle != INVALID_HANDLE_VALUE)
		return newFind->wfd.cFileName;

#elif defined(__SWITCH__)
	char* slash = strrchr(newFind->wildcard, '/');

	newFind->dir = NULL;
	newFind->currentName = NULL;

	if (slash)
	{
		*slash = '\0';
		newFind->dirPath = (slash == newFind->wildcard) ? "/" : newFind->wildcard;
		newFind->pattern = slash + 1;
	}
	else
	{
		newFind->dirPath = ".";
		newFind->pattern = newFind->wildcard;
	}

	newFind->dir = opendir(newFind->dirPath);
	if (newFind->dir)
		return FS_FindNextMatch(newFind);
#else // POSIX
	newFind->index = -1;

	if (glob(newFind->wildcard, 0, NULL, &newFind->gl) == 0 && newFind->gl.gl_pathc > 0)
	{
		newFind->pathlen = strchr(newFind->wildcard, '.') - newFind->wildcard;
		newFind->index = 0;
		return newFind->gl.gl_pathv[newFind->index] + newFind->pathlen;
	}
#endif // _WIN32

	// delete if no luck
	FS_FindClose(newFind);
	*findData = NULL;

	return nullptr;
}

const char* FS_FindNext(FS_FINDDATA* findData)
{
	if (!findData)
		return nullptr;

#ifdef _WIN32
	if (!::FindNextFileA(findData->fileHandle, &findData->wfd))
		return nullptr;
#elif defined(__SWITCH__)
	return FS_FindNextMatch(findData);
#else
	if (findData->index < 0 || findData->index >= findData->gl.gl_pathc)
		return nullptr;
#endif // _WIN32

#ifdef _WIN32
	return findData->wfd.cFileName;
#elif defined(__SWITCH__)
	return nullptr;
#else
	findData->index++;
	return findData->gl.gl_pathv[findData->index] + findData->pathlen;
#endif // _WIN32
}

void FS_FindClose(FS_FINDDATA* findData)
{
	if (!findData)
		return;

	free(findData->wildcard);

#ifdef _WIN32
	if(findData->fileHandle = INVALID_HANDLE_VALUE)
		FindClose(findData->fileHandle);
#elif defined(__SWITCH__)
	if (findData->dir)
		closedir(findData->dir);
#else
	if (findData->index >= 0)
		globfree(&findData->gl);
#endif // _WIN32
	delete findData;
}

bool FS_FindIsDirectory(FS_FINDDATA* findData)
{
	if (!findData)
		return false;

#ifdef _WIN32
	return (findData->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#elif defined(__SWITCH__)
	struct stat st;
	char currentPath[1024];

	if (!findData->currentName)
		return false;

	if (snprintf(currentPath, sizeof(currentPath), "%s/%s", findData->dirPath, findData->currentName) >= sizeof(currentPath))
		return false;

	if (stat(currentPath, &st) == 0)
		return (st.st_mode & S_IFDIR) > 0;

	return false;
#else
	struct stat st;

	if (stat(findData->gl.gl_pathv[findData->index], &st) == 0)
	{
		return (st.st_mode & S_IFDIR) > 0;
	}

	return false;
#endif // _WIN32
}
