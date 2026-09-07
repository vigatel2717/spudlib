<<<<<<< HEAD
#include "spudfiles.h"

#include <sys/stat.h>
=======
#define _POSIX_C_SOURCE 200809L

#include "spudcore.h"
#include "spudfiles.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#if __cplusplus
extern "C" {
#endif // __cplusplus

struct sfs_file_t {
#if _DEBUG
	const char *debug_name;
#endif
	int fd;
};

// --------------------------------------------------------------------------
// File open / close
// --------------------------------------------------------------------------

SPUDRESULT
sfs_file_open(
    SFS_FILE_OPEN_ATTRIBUTES open_attribs,
    sfs_file *out_file) {
	if (!open_attribs.str_file_path || open_attribs.str_file_path[0] == '\0')
		return SPUDRESULT_SFS_NULL_PATH;
	if (!out_file)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	int flags = 0;

	switch (open_attribs.access_mode) {
	case SFS_EFILE_ACCESS_MODE_READ:
		flags = O_RDONLY;
		break;
	case SFS_EFILE_ACCESS_MODE_OVERWRITE:
		flags = O_WRONLY | O_CREAT | O_TRUNC;
		break;
	case SFS_EFILE_ACCESS_MODE_APPEND:
		flags = O_WRONLY | O_CREAT | O_APPEND;
		break;
	case SFS_EFILE_ACCESS_MODE_READ_UPDATE:
		flags = O_RDWR;
		break;
	case SFS_EFILE_ACCESS_MODE_OVERWRITE_UPDATE:
		flags = O_RDWR | O_CREAT | O_TRUNC;
		break;
	case SFS_EFILE_ACCESS_MODE_APPEND_UPDATE:
		flags = O_RDWR | O_CREAT | O_APPEND;
		break;
	default:
		return SPUDRESULT_DESC_INVALID_PARAMETERS;
	}

	int fd = open(open_attribs.str_file_path, flags, 0644);
	if (fd < 0)
		return SPUDRESULT_GENERAL_FAILURE;

	struct sfs_file_t *f = (struct sfs_file_t *)malloc(sizeof(struct sfs_file_t));
	if (!f) {
		close(fd);
		return SPUDRESULT_OUT_OF_MEMORY;
	}
	f->fd     = fd;
	*out_file = f;
	return SPUD_SUCCESS;
}

SPUDRESULT sfs_file_release(sfs_file file) {
	if (!file)
		return SPUDRESULT_SFS_INVALID_FILE;
	close(file->fd);
	free(file);
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File I/O
// --------------------------------------------------------------------------

SPUDRESULT sfs_file_read(
    sfs_file file,
    void *data,
    uint64_t size) {
	if (!file)
		return SPUDRESULT_SFS_INVALID_FILE;
	if (!data)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	uint8_t *dst       = (uint8_t *)data;
	uint64_t remaining = size;

	while (remaining > 0) {
		ssize_t n = read(file->fd, dst, remaining);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			return SPUDRESULT_GENERAL_FAILURE;
		}
		if (n == 0)
			break; // EOF
		dst += n;
		remaining -= (uint64_t)n;
	}
	return SPUD_SUCCESS;
}

SPUDRESULT sfs_file_write(
    sfs_file file,
    const void *data,
    uint64_t size) {
	if (!file)
		return SPUDRESULT_SFS_INVALID_FILE;
	if (!data)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	const uint8_t *src = (const uint8_t *)data;
	uint64_t remaining = size;

	while (remaining > 0) {
		ssize_t n = write(file->fd, src, remaining);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			return SPUDRESULT_GENERAL_FAILURE;
		}
		src += n;
		remaining -= (uint64_t)n;
	}
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File position / size
// --------------------------------------------------------------------------

SPUDRESULT sfs_file_get_size(
    sfs_file file,
    uint64_t *out_size) {
	if (!file)
		return SPUDRESULT_SFS_INVALID_FILE;
	if (!out_size)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	struct stat st;
	if (fstat(file->fd, &st) != 0)
		return SPUDRESULT_GENERAL_FAILURE;

	*out_size = (uint64_t)st.st_size;
	return SPUD_SUCCESS;
}

SPUDRESULT sfs_file_get_pos(
    sfs_file file,
    uint64_t *out_pos) {
	if (!file)
		return SPUDRESULT_SFS_INVALID_FILE;
	if (!out_pos)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	off_t pos = lseek(file->fd, 0, SEEK_CUR);
	if (pos < 0)
		return SPUDRESULT_GENERAL_FAILURE;

	*out_pos = (uint64_t)pos;
	return SPUD_SUCCESS;
}

SPUDRESULT
sfs_file_set_pos(
    sfs_file file,
    uint64_t offset,
    SFS_FILE_POS_ORIGIN origin) {
	if (!file)
		return SPUDRESULT_SFS_INVALID_FILE;

	int whence;
	switch (origin) {
	case SFS_FILE_POS_ORIGIN_START:
		whence = SEEK_SET;
		break;
	case SFS_FILE_POS_ORIGIN_CURRENT:
		whence = SEEK_CUR;
		break;
	case SFS_FILE_POS_ORIGIN_END:
		whence = SEEK_END;
		break;
	default:
		return SPUDRESULT_DESC_INVALID_PARAMETERS;
	}

	if (lseek(file->fd, (off_t)offset, whence) < 0)
		return SPUDRESULT_GENERAL_FAILURE;

	return SPUD_SUCCESS;
}
>>>>>>> 5a737da092100dd1797b106187fded965744c1f2

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

<<<<<<< HEAD
bool sfs_directory_exists(const char *dir_path) {
	if (!dir_path || dir_path[0] == '\0')
		return false;
	struct stat st;
	if (stat(dir_path, &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}
=======
SPUDRESULT sfs_set_working_directory(const char *dir) {
	if (!dir || dir[0] == '\0')
		return SPUDRESULT_SFS_NULL_PATH;
	if (chdir(dir) != 0)
		return SPUDRESULT_GENERAL_FAILURE;
	return SPUD_SUCCESS;
}

const char *sfs_get_working_directory() {
	static char buf[PATH_MAX];
	if (!getcwd(buf, sizeof(buf)))
		return NULL;
	return buf;
}

// --------------------------------------------------------------------------
// Directory creation
// --------------------------------------------------------------------------

SPUDRESULT sfs_create_directory(const char *path) {
	if (path == NULL || path[0] == '\0')
		return SPUDRESULT_SFS_NULL_PATH;

	char dir_path[PATH_MAX];
	strncpy(dir_path, path, PATH_MAX - 1);
	dir_path[PATH_MAX - 1] = '\0';

	// normalize slashes
	char *dp = dir_path;
	sfs_correct_slashes(&dp, SFS_SLASH);

	char *search = dir_path;
	char *slash_pos;

	do {
		slash_pos = strchr(search, SFS_SLASH);

		char parent[PATH_MAX];
		if (slash_pos != NULL) {
			uint64_t len = (uint64_t)(slash_pos - dir_path);
			if (len == 0) {
				// leading slash (absolute path root) - skip
				search = slash_pos + 1;
				continue;
			}
			strncpy(parent, dir_path, len);
			parent[len] = '\0';
			search      = slash_pos + 1;
		} else {
			strncpy(parent, dir_path, PATH_MAX - 1);
			parent[PATH_MAX - 1] = '\0';
		}

		if (parent[0] == '\0')
			continue;

		if (mkdir(parent, 0755) != 0) {
			if (errno != EEXIST)
				return SPUDRESULT_GENERAL_FAILURE;

			struct stat st;
			if (stat(parent, &st) != 0 || !S_ISDIR(st.st_mode))
				return SPUDRESULT_GENERAL_FAILURE;
		}
	} while (slash_pos != NULL);

	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File dialogs
// --------------------------------------------------------------------------

// Runs `argv[0]` (searched on PATH, never through a shell) with the given
// argv, captures its trimmed stdout into a static buffer, and returns it.
// Returns NULL if the binary can't be found/run, exits non-zero, or prints
// nothing (e.g. the user cancelled the dialog).
static const char *run_dialog_helper(char *const argv[]) {
	static char result[PATH_MAX * 4];

	int pipefd[2];
	if (pipe(pipefd) != 0)
		return NULL;

	pid_t pid = fork();
	if (pid < 0) {
		close(pipefd[0]);
		close(pipefd[1]);
		return NULL;
	}

	if (pid == 0) {
		// child
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		int devnull = open("/dev/null", O_WRONLY);
		if (devnull >= 0)
			dup2(devnull, STDERR_FILENO);
		execvp(argv[0], argv);
		_exit(127);
	}

	// parent
	close(pipefd[1]);

	uint64_t total = 0;
	for (;;) {
		ssize_t n = read(pipefd[0], result + total, sizeof(result) - 1 - total);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			break;
		}
		if (n == 0)
			break;
		total += (uint64_t)n;
		if (total >= sizeof(result) - 1)
			break;
	}
	close(pipefd[0]);
	result[total] = '\0';

	int status = 0;
	waitpid(pid, &status, 0);

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return NULL;

	// strip trailing newline emitted by the dialog helpers
	while (total > 0 && (result[total - 1] == '\n' || result[total - 1] == '\r'))
		result[--total] = '\0';

	if (total == 0)
		return NULL;

	return result;
}

static bool dialog_helper_available(const char *name) {
	char probe[PATH_MAX];
	const char *dirs = getenv("PATH");
	if (!dirs)
		return false;

	char path_copy[PATH_MAX * 4];
	strncpy(path_copy, dirs, sizeof(path_copy) - 1);
	path_copy[sizeof(path_copy) - 1] = '\0';

	char *saveptr = NULL;
	for (char *dir = strtok_r(path_copy, ":", &saveptr); dir; dir = strtok_r(NULL, ":", &saveptr)) {
		int n = snprintf(probe, sizeof(probe), "%s/%s", dir, name);
		if (n > 0 && (size_t)n < sizeof(probe) && access(probe, X_OK) == 0)
			return true;
	}
	return false;
}

const char *sfs_file_dialog(SFS_FILE_DIALOG_ATTRIBUTES attributes) {
	const char *title = attributes.title ? attributes.title : "";

	if (dialog_helper_available("zenity")) {
		char *argv[8];
		int argc     = 0;
		argv[argc++] = "zenity";
		argv[argc++] = "--file-selection";
		if (attributes.type == SFS_FILE_DIALOG_TYPE_SAVE)
			argv[argc++] = "--save";
		if (attributes.flags & SFS_FILE_DIALOG_FLAG_OVERWRITE_PROMPT)
			argv[argc++] = "--confirm-overwrite";
		static char title_arg[512];
		snprintf(title_arg, sizeof(title_arg), "--title=%s", title);
		argv[argc++] = title_arg;
		argv[argc]   = NULL;
		return run_dialog_helper(argv);
	}

	if (dialog_helper_available("kdialog")) {
		char *argv[6];
		int argc     = 0;
		argv[argc++] = "kdialog";
		argv[argc++] = (char *)(attributes.type == SFS_FILE_DIALOG_TYPE_SAVE ? "--getsavefilename" : "--getopenfilename");
		static char title_arg[512];
		snprintf(title_arg, sizeof(title_arg), "--title=%s", title);
		argv[argc++] = title_arg;
		argv[argc]   = NULL;
		return run_dialog_helper(argv);
	}

	return NULL;
}

const char *sfs_open_folder_dialog(const char *title) {
	if (!title)
		title = "Select Folder";

	if (dialog_helper_available("zenity")) {
		char *argv[6];
		int argc     = 0;
		argv[argc++] = "zenity";
		argv[argc++] = "--file-selection";
		argv[argc++] = "--directory";
		static char title_arg[512];
		snprintf(title_arg, sizeof(title_arg), "--title=%s", title);
		argv[argc++] = title_arg;
		argv[argc]   = NULL;
		return run_dialog_helper(argv);
	}

	if (dialog_helper_available("kdialog")) {
		char *argv[5];
		int argc     = 0;
		argv[argc++] = "kdialog";
		argv[argc++] = "--getexistingdirectory";
		static char title_arg[512];
		snprintf(title_arg, sizeof(title_arg), "--title=%s", title);
		argv[argc++] = title_arg;
		argv[argc]   = NULL;
		return run_dialog_helper(argv);
	}

	return NULL;
}

#if __cplusplus
}
#endif // __cplusplus
>>>>>>> 5a737da092100dd1797b106187fded965744c1f2
