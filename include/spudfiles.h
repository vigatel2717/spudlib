//
// Created by Nathan on 5/9/2026.
// SFS = SpudFiles
//

#ifndef SPUDLIB_SPUDFILES_H
#define SPUDLIB_SPUDFILES_H

#include "spudcore.h"
#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct spudblob_t *spudblob;
typedef struct sfs_file_t *sfs_file;
typedef struct sfs_directory_t *sfs_directory;

typedef enum SFS_EFILE_ACCESS_MODE {        // EFileAccessMode
	SFS_EFILE_ACCESS_MODE_READ,             // Read,
	SFS_EFILE_ACCESS_MODE_OVERWRITE,        // Overwrite
	SFS_EFILE_ACCESS_MODE_APPEND,           // Append
	SFS_EFILE_ACCESS_MODE_READ_UPDATE,      // ReadUpdate
	SFS_EFILE_ACCESS_MODE_OVERWRITE_UPDATE, // OverwriteUpdate
	SFS_EFILE_ACCESS_MODE_APPEND_UPDATE     // AppendUpdate
} SFS_EFILE_ACCESS_MODE;

typedef struct SFS_FILE_OPEN_ATTRIBUTES { // FileOpenAttribs
	const char *str_file_path;
	SFS_EFILE_ACCESS_MODE access_mode;
} SFS_FILE_OPEN_ATTRIBUTES;

typedef uint32_t SFS_FILE_DIALOG_FLAGS;
enum {
	SFS_FILE_DIALOG_FLAG_NONE = 0x000,

	/// Prevents the system from adding a link to the selected file in the file
	/// system directory that contains the user's most recently used documents.
	SFS_FILE_DIALOG_FLAG_DONT_ADD_TO_RECENT = 0x001,

	/// Only existing files can be opened
	SFS_FILE_DIALOG_FLAG_FILE_MUST_EXIST = 0x002,

	/// Restores the current directory to its original value if the user changed
	/// the directory while searching for files.
	SFS_FILE_DIALOG_FLAG_NO_CHANGE_DIR = 0x004,

	/// Causes the Save As dialog box to show a message box if the selected file
	/// already exists.
	SFS_FILE_DIALOG_FLAG_OVERWRITE_PROMPT = 0x008
};

typedef uint32_t SFS_FILE_DIALOG_TYPE;
enum { SFS_FILE_DIALOG_TYPE_OPEN, SFS_FILE_DIALOG_TYPE_SAVE };

typedef struct SFS_FIND_FILE_DATA { // FindFileData
	const char *name;
	bool is_directory;
} SFS_FIND_FILE_DATA;

typedef struct SFS_FILE_DIALOG_ATTRIBUTES { // FileDialogAttribs
	SFS_FILE_DIALOG_TYPE type;
	SFS_FILE_DIALOG_FLAGS flags;
	const char *title;
	const char *filter;
} SFS_FILE_DIALOG_ATTRIBUTES;

SFS_FILE_DIALOG_ATTRIBUTES
sfs_default_file_dialog_attributes(SFS_FILE_DIALOG_TYPE type);

typedef enum SFS_FILE_POS_ORIGIN { // FilePosOrigin
	SFS_FILE_POS_ORIGIN_START,     // Start
	SFS_FILE_POS_ORIGIN_CURRENT,   // Curr
	SFS_FILE_POS_ORIGIN_END        // End
} SFS_FILE_POS_ORIGIN;

#define SFS_UNIX_SLASH '/'
#define SFS_WINDOWS_SLASH '\\'
#if SPUDLIB_PLATFORM_WIN32
#define SFS_SLASH SFS_WINDOWS_SLASH
#else
#define SFS_SLASH SFS_UNIX_SLASH
#endif

// SPUDRESULT sfs_file_read(sfs_file file, spudblob data);
SPUDRESULT sfs_file_read(sfs_file file, void *data, uint64_t size);
SPUDRESULT sfs_file_write(sfs_file file, const void *data, uint64_t size);
SPUDRESULT sfs_file_get_size(sfs_file file, uint64_t *out_size);
SPUDRESULT sfs_file_get_pos(sfs_file file, uint64_t *out_pos);
SPUDRESULT
sfs_file_set_pos(sfs_file file, uint64_t offset, SFS_FILE_POS_ORIGIN origin);

SPUDRESULT sfs_create_directory(const char *path);

SPUDRESULT
sfs_file_open(SFS_FILE_OPEN_ATTRIBUTES open_attribs, sfs_file *out_file);
SPUDRESULT sfs_file_release(sfs_file file);
bool sfs_file_exists(const char *file_path);
bool sfs_directory_exists(const char *dir_path);
SPUDRESULT sfs_set_working_directory(const char *dir);
const char *sfs_get_working_directory();
bool sfs_is_slash(char c);
bool sfs_correct_slashes(char **in_out_path, char slash);
bool sfs_get_path_components(
    const char *path, const char **out_directory, const char **out_filename);
bool sfs_is_absolute(const char *path);
bool sfs_split_path(
    const char *in_path,
    bool simplify,
    const char **out_paths,
    uint32_t *out_path_count);
bool sfs_build_path_from_components(const char **in_components, char slash);\
const char *sfs_simplify_path(const char *path, char slash);
const char *sfs_get_relative_path(
    const char *path_from,
    bool is_from_directory,
    const char *path_to,
    bool is_to_directory,
    char slash);
const char *sfs_file_dialog_multi_path(
    const char *path1,
    const char *path2,
    uint32_t prefix1_length,
    uint32_t prefix2_length);
const char *sfs_file_dialog(SFS_FILE_DIALOG_ATTRIBUTES attributes);
const char *sfs_open_folder_dialog(const char *title);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPUDLIB_SPUDFILES_H
