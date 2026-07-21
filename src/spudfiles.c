
#include "spudfiles.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SFS_FILE_DIALOG_ATTRIBUTES
sfs_default_file_dialog_attributes(SFS_FILE_DIALOG_TYPE type) {
	SFS_FILE_DIALOG_ATTRIBUTES result = {0};
	result.type                       = SFS_FILE_DIALOG_TYPE_OPEN;
	result.flags                      = SFS_FILE_DIALOG_FLAG_NONE;
	result.title                      = NULL;
	result.filter                     = NULL;
	switch (type) {
	case SFS_FILE_DIALOG_TYPE_OPEN:
		result.flags = SFS_FILE_DIALOG_FLAG_DONT_ADD_TO_RECENT |
		               SFS_FILE_DIALOG_FLAG_FILE_MUST_EXIST |
		               SFS_FILE_DIALOG_FLAG_NO_CHANGE_DIR;
		break;

	case SFS_FILE_DIALOG_TYPE_SAVE:
		result.flags = SFS_FILE_DIALOG_FLAG_DONT_ADD_TO_RECENT |
		               SFS_FILE_DIALOG_FLAG_OVERWRITE_PROMPT |
		               SFS_FILE_DIALOG_FLAG_NO_CHANGE_DIR;
		break;
	}
	return result;
}

bool sfs_is_slash(char c) { return c == '/' || c == '\\'; }

bool sfs_correct_slashes(char **in_out_path, char slash) {
	if (!in_out_path || !*in_out_path)
		return false;
	if (slash == '\0')
		slash = SFS_SLASH;
	char other = (slash == '/') ? '\\' : '/';
	for (char *p = *in_out_path; *p; p++) {
		if (*p == other)
			*p = slash;
	}
	return true;
}

bool sfs_is_absolute(const char *path) {
	if (!path || !path[0])
		return false;
	// Unix absolute path
	if (path[0] == '/')
		return true;
	// UNC path: \\server\share
	if (path[0] == '\\' && path[1] == '\\')
		return true;
	// Windows drive-absolute: C:\ or C:/
	if (path[1] == ':' && sfs_is_slash(path[2]))
		return true;
	return false;
}

bool sfs_get_path_components(
    const char *path, const char **out_directory, const char **out_filename) {
	static char dir_buf[4096];
	static char file_buf[512];

	if (!path)
		return false;

	const char *last_slash = NULL;
	for (const char *p = path; *p; p++) {
		if (sfs_is_slash(*p))
			last_slash = p;
	}

	if (last_slash == NULL) {
		if (out_directory) {
			dir_buf[0]     = '\0';
			*out_directory = dir_buf;
		}
		if (out_filename) {
			strncpy(file_buf, path, sizeof(file_buf) - 1);
			file_buf[sizeof(file_buf) - 1] = '\0';
			*out_filename                  = file_buf;
		}
	} else {
		if (out_directory) {
			size_t len = (size_t)(last_slash - path);
			if (len == 0)
				len = 1;
			if (len >= sizeof(dir_buf))
				len = sizeof(dir_buf) - 1;
			strncpy(dir_buf, path, len);
			dir_buf[len]   = '\0';
			*out_directory = dir_buf;
		}
		if (out_filename) {
			strncpy(file_buf, last_slash + 1, sizeof(file_buf) - 1);
			file_buf[sizeof(file_buf) - 1] = '\0';
			*out_filename                  = file_buf;
		}
	}
	return true;
}

// Extracts the path prefix (drive letter "C:" or leading slashes for UNC/unix).
// Returns the length of the prefix and copies it into out_prefix
// (null-terminated).
static uint32_t
extract_path_prefix(const char *path, char *out_prefix, uint32_t prefix_cap) {
	if (!path || prefix_cap == 0)
		return 0;
	out_prefix[0] = '\0';
	// Windows drive: "C:"
	if (path[1] == ':') {
		if (prefix_cap < 3)
			return 0;
		out_prefix[0] = path[0];
		out_prefix[1] = path[1];
		out_prefix[2] = '\0';
		return 2;
	}
	// UNC: "\\"
	if (path[0] == '\\' && path[1] == '\\') {
		// copy up through the share name: \\server\share
		uint32_t i           = 2;
		uint32_t slash_count = 0;
		while (path[i] && slash_count < 2) {
			if (sfs_is_slash(path[i]))
				slash_count++;
			i++;
		}
		if (i >= prefix_cap)
			i = prefix_cap - 1;
		strncpy(out_prefix, path, i);
		out_prefix[i] = '\0';
		return i;
	}
	return 0;
}

const char *sfs_simplify_path(const char *path, char slash) {
	static char result[4096];
	static char work[4096];
	// component stack stored as offsets into result
	static uint32_t stack[256];
	uint32_t stack_top = 0;

	if (!path)
		return "";
	if (slash == '\0')
		slash = SFS_SLASH;

	char prefix[64]     = {0};
	uint32_t prefix_len = extract_path_prefix(path, prefix, sizeof(prefix));

	strncpy(work, path + prefix_len, sizeof(work) - 1);
	work[sizeof(work) - 1] = '\0';

	// tokenize into components
	uint32_t result_len = 0;
	// write prefix first
	strncpy(result, prefix, sizeof(result) - 1);
	result_len = prefix_len;

	char *p = work;
	while (*p) {
		while (*p && sfs_is_slash(*p))
			p++;
		if (!*p)
			break;

		char *start = p;
		while (*p && !sfs_is_slash(*p))
			p++;
		uint32_t comp_len = (uint32_t)(p - start);

		if (comp_len == 1 && start[0] == '.') {
			// skip "."
		} else if (comp_len == 2 && start[0] == '.' && start[1] == '.') {
			// pop last component if possible
			if (stack_top > 0) {
				stack_top--;
				result_len         = stack[stack_top];
				result[result_len] = '\0';
			}
		} else {
			// push component
			if (result_len > prefix_len) {
				if (result_len + 1 < sizeof(result)) {
					result[result_len++] = slash;
				}
			}
			stack[stack_top++] = result_len;
			if (result_len + comp_len < sizeof(result)) {
				memcpy(result + result_len, start, comp_len);
				result_len += comp_len;
				result[result_len] = '\0';
			}
		}
	}

	if (result_len == 0) {
		result[0] = '.';
		result[1] = '\0';
	}

	return result;
}

bool sfs_split_path(
    const char *in_path,
    bool simplify,
    const char **out_paths,
    uint32_t *out_path_count) {
	static char path_buf[4096];

	if (!in_path || !out_path_count)
		return false;

	const char *src =
	    simplify ? sfs_simplify_path(in_path, SFS_SLASH) : in_path;
	strncpy(path_buf, src, sizeof(path_buf) - 1);
	path_buf[sizeof(path_buf) - 1] = '\0';

	uint32_t count    = 0;
	uint32_t capacity = *out_path_count;
	char *p           = path_buf;

	while (*p) {
		while (*p && sfs_is_slash(*p))
			p++;
		if (!*p)
			break;

		char *start = p;
		while (*p && !sfs_is_slash(*p))
			p++;
		if (*p)
			*p++ = '\0';

		if (out_paths && count < capacity) {
			out_paths[count] = start;
		}
		count++;
	}

	*out_path_count = count;
	return true;
}

// Takes a null-terminated array where in_components[0] is a writable output
// buffer large enough to hold the joined result, and in_components[1..] are the
// path components to join. Returns true on success.
bool sfs_build_path_from_components(const char **in_components, char slash) {
	if (!in_components || !in_components[0])
		return false;
	if (slash == '\0')
		slash = SFS_SLASH;

	char *out        = (char *)in_components[0];
	uint32_t out_pos = 0;

	for (uint32_t i = 1; in_components[i]; i++) {
		uint32_t comp_len = (uint32_t)strlen(in_components[i]);
		if (out_pos > 0)
			out[out_pos++] = slash;
		memcpy(out + out_pos, in_components[i], comp_len);
		out_pos += comp_len;
	}
	out[out_pos] = '\0';
	return true;
}

const char *sfs_get_relative_path(
    const char *path_from,
    bool is_from_directory,
    const char *path_to,
    bool is_to_directory,
    char slash) {
	static char result[4096];
	static char comps_from_buf[4096];
	static char comps_to_buf[4096];
	static const char *comps_from[256];
	static const char *comps_to[256];

	if (!path_from || !path_to)
		return "";
	if (slash == '\0')
		slash = SFS_SLASH;

	// Simplify both paths first
	const char *simplified_from = sfs_simplify_path(path_from, slash);
	strncpy(comps_from_buf, simplified_from, sizeof(comps_from_buf) - 1);
	comps_from_buf[sizeof(comps_from_buf) - 1] = '\0';

	const char *simplified_to = sfs_simplify_path(path_to, slash);
	strncpy(comps_to_buf, simplified_to, sizeof(comps_to_buf) - 1);
	comps_to_buf[sizeof(comps_to_buf) - 1] = '\0';

	// Split into components
	uint32_t from_count = 256;
	uint32_t to_count   = 256;

	// Use local arrays to avoid clobbering static state in sfs_split_path
	{
		char *p    = comps_from_buf;
		from_count = 0;
		while (*p) {
			while (*p && sfs_is_slash(*p))
				p++;
			if (!*p)
				break;
			comps_from[from_count++] = p;
			while (*p && !sfs_is_slash(*p))
				p++;
			if (*p)
				*p++ = '\0';
		}
	}
	{
		char *p  = comps_to_buf;
		to_count = 0;
		while (*p) {
			while (*p && sfs_is_slash(*p))
				p++;
			if (!*p)
				break;
			comps_to[to_count++] = p;
			while (*p && !sfs_is_slash(*p))
				p++;
			if (*p)
				*p++ = '\0';
		}
	}

	// If path_from is a file, the last component is the filename — ignore it
	uint32_t from_dir_count =
	    is_from_directory ? from_count : (from_count > 0 ? from_count - 1 : 0);

	// Find common prefix length
	uint32_t common = 0;
	uint32_t min    = from_dir_count < to_count ? from_dir_count : to_count;
	while (common < min && strcmp(comps_from[common], comps_to[common]) == 0) {
		common++;
	}

	uint32_t result_len = 0;
	result[0]           = '\0';

	// Emit ".." for each remaining component in from after the common prefix
	for (uint32_t i = common; i < from_dir_count; i++) {
		if (result_len > 0)
			result[result_len++] = slash;
		result[result_len++] = '.';
		result[result_len++] = '.';
		result[result_len]   = '\0';
	}

	// Emit remaining components in to after the common prefix
	uint32_t to_end = is_to_directory ? to_count : to_count;
	for (uint32_t i = common; i < to_end; i++) {
		if (result_len > 0)
			result[result_len++] = slash;
		uint32_t comp_len = (uint32_t)strlen(comps_to[i]);
		memcpy(result + result_len, comps_to[i], comp_len);
		result_len += comp_len;
		result[result_len] = '\0';
	}

	if (result_len == 0) {
		result[0] = '.';
		result[1] = '\0';
	}

	return result;
}

// Combines path1 (up to prefix1_length chars) with path2 (skipping
// prefix2_length chars). Used when a multi-select file dialog returns a
// directory + individual filenames.
const char *sfs_file_dialog_multi_path(
    const char *path1,
    const char *path2,
    uint32_t prefix1_length,
    uint32_t prefix2_length) {
	static char result[4096];

	if (!path1 || !path2)
		return "";

	uint32_t len1 =
	    prefix1_length > 0 ? prefix1_length : (uint32_t)strlen(path1);
	const char *tail = path2 + prefix2_length;

	if (len1 >= sizeof(result))
		len1 = sizeof(result) - 1;
	memcpy(result, path1, len1);
	result[len1] = '\0';

	if (*tail) {
		uint32_t pos = len1;
		if (pos > 0 && !sfs_is_slash(result[pos - 1])) {
			if (pos + 1 < sizeof(result))
				result[pos++] = SFS_SLASH;
		}
		uint32_t tail_len = (uint32_t)strlen(tail);
		if (pos + tail_len >= sizeof(result))
			tail_len = sizeof(result) - pos - 1;
		memcpy(result + pos, tail, tail_len);
		result[pos + tail_len] = '\0';
	}

	return result;
}

#ifdef __cplusplus
}
#endif // __cplusplus
