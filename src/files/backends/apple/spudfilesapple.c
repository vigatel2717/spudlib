#include "spudfiles.h"

#include <sys/stat.h>

// --------------------------------------------------------------------------
// File / directory queries
// --------------------------------------------------------------------------

bool sfs_file_exists(const char *file_path) {
	if (!file_path || file_path[0] == '\0')
		return false;
	struct stat st;
	if (stat(file_path, &st) != 0)
		return false;
	return !S_ISDIR(st.st_mode);
}

bool sfs_directory_exists(const char *dir_path) {
	if (!dir_path || dir_path[0] == '\0')
		return false;
	struct stat st;
	if (stat(dir_path, &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}
