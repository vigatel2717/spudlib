//
// Created by Nathan on 5/9/2026.
// SFS = SpudFiles
//

#ifndef SPUDLIB_SPUDFILES_H
#define SPUDLIB_SPUDFILES_H

#include "spudcore.h"

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

/*


class BasicFile
{
public:
    BasicFile(const FileOpenAttribs& OpenAttribs);
    virtual ~BasicFile();

    const String& GetPath() { return m_Path; }

protected:
    String GetOpenModeStr();

    const String          m_Path;
    const FileOpenAttribs m_OpenAttribs;
};


struct FileDialogAttribs
{
    FILE_DIALOG_TYPE  Type  = FILE_DIALOG_TYPE_OPEN;
    FILE_DIALOG_FLAGS Flags = FILE_DIALOG_FLAG_NONE;

    const char* Title  = nullptr;
    const char* Filter = nullptr;

    FileDialogAttribs() noexcept {}

    explicit FileDialogAttribs(FILE_DIALOG_TYPE _Type) noexcept :
        Type{_Type}
    {
        switch (Type)
        {
            case FILE_DIALOG_TYPE_OPEN:
                Flags = FILE_DIALOG_FLAG_DONT_ADD_TO_RECENT |
FILE_DIALOG_FLAG_FILE_MUST_EXIST | FILE_DIALOG_FLAG_NO_CHANGE_DIR; break;

            case FILE_DIALOG_TYPE_SAVE:
                Flags = FILE_DIALOG_FLAG_DONT_ADD_TO_RECENT |
FILE_DIALOG_FLAG_OVERWRITE_PROMPT | FILE_DIALOG_FLAG_NO_CHANGE_DIR; break;
        }
    }
};



/// Basic platform-specific file system functions
struct BasicFileSystem
{
public:
    static constexpr Char UnixSlash = '/';
    static constexpr Char WinSlash  = '\\';

#if PLATFORM_WIN32 || PLATFORM_UNIVERSAL_WINDOWS
    static constexpr Char SlashSymbol = WinSlash;
#else
    static constexpr Char SlashSymbol = UnixSlash;
#endif

    using SearchFilesResult = std::vector<FindFileData>;

    static BasicFile* OpenFile(FileOpenAttribs& OpenAttribs);
    static void       ReleaseFile(BasicFile*);

    static bool FileExists(const Char* strFilePath);

    static void SetWorkingDirectory(const Char* strWorkingDir) {
m_strWorkingDirectory = strWorkingDir; }

    static const String& GetWorkingDirectory() { return m_strWorkingDirectory; }

    static bool IsSlash(Char c)
    {
        return c == UnixSlash || c == WinSlash;
    }

    static void CorrectSlashes(String& Path, Char Slash = 0);

    static void GetPathComponents(const String& Path,
                                  String*       Directory,
                                  String*       FileName);

    static bool IsPathAbsolute(const Char* Path);

    /// Splits path into individual components optionally simplifying it.
    ///
    /// If Simplify is true:
    ///     - Removes redundant slashes (`a///b -> a/b`)
    ///     - Removes redundant `.` (`a/./b -> a/b`)
    ///     - Collapses `..` (`a/b/../c -> a/c`)
    static std::vector<String> SplitPath(const Char* Path, bool Simplify);

    /// Builds a path from the given components.
    static std::string BuildPathFromComponents(const std::vector<String>&
Components, Char Slash = 0);

    /// Simplifies the path.

    /// The function performs the following path simplifications:
    /// - Normalizes slashes using the given slash symbol (`a\b/c -> a/b/c`)
    /// - Removes redundant slashes (`a///b -> a/b`)
    /// - Removes redundant `.` (`a/./b `-> `a/b`)
    /// - Collapses `..` (`a/b/../c -> a/c`)
    /// - Removes trailing slashes (`/a/b/c/ -> /a/b/c`)
    /// - When 'Slash' is Windows slash (`\`), removes leading slashes (`\a\b\c
-> a\b\c`) static std::string SimplifyPath(const Char* Path, Char Slash = 0);


    /// Splits a list of paths separated by a given separator and calls a user
callback for every individual path.
    /// Empty paths are skipped.
    template <typename CallbackType>
    static void SplitPathList(const Char* PathList, CallbackType Callback, const
char Separator = ';')
    {
        if (PathList == nullptr)
            return;

        const auto* Pos = PathList;
        while (*Pos != '\0')
        {
            while (*Pos != '\0' && *Pos == Separator)
                ++Pos;
            if (*Pos == '\0')
                break;

            const auto* End = Pos + 1;
            while (*End != '\0' && *End != Separator)
                ++End;

            if (!Callback(Pos, static_cast<size_t>(End - Pos)))
                break;

            Pos = End;
        }
    }

    /// Returns a relative path from one file or folder to another.

    /// \param [in]  PathFrom        - Path that defines the start of the
relative path.
    ///                                Must not be null.
    /// \param [in]  IsFromDirectory - Indicates if PathFrom is a directory.
    /// \param [in]  PathTo          - Path that defines the endpoint of the
relative path.
    ///                                Must not be null.
    /// \param [in]  IsToDirectory   - Indicates if PathTo is a directory.
    /// \param [in]  Slash           - Slash symbol to use in the returned path.
    ///                                If 0, platform-specific slash is used.
    ///
    /// \return                        Relative path from PathFrom to PathTo.
    ///                                If no relative path exists, PathFrom is
returned. static std::string GetRelativePath(const Char* PathFrom, bool
IsFromDirectory, const Char* PathTo, bool        IsToDirectory, Char Slash = 0);

    /// Returns the longest common *path-component* prefix of two paths.
    ///
    /// Components are separated by '/' or '\\'. Prefix matching is done
    /// component-by-component (no partial component matches). If one path is
absolute
    /// and the other is relative, the prefix is empty. Repeated separators are
ignored
    /// for matching, but the returned lengths count characters in the original
strings,
    /// so Prefix1Len and Prefix2Len may differ.
    ///
    /// \param [in]  Path1         - First path. Must not be null.
    /// \param [in]  Path2         - Second path. Must not be null.
    /// \param [out] Prefix1Len    - Length of the common prefix in Path1.
    /// \param [out] Prefix2Len    - Length of the common prefix in Path2.
    static void GetCommonPathPrefix(const char* Path1,
                                    const char* Path2,
                                    size_t&     Prefix1Len,
                                    size_t&     Prefix2Len);

    static std::string FileDialog(const FileDialogAttribs& DialogAttribs);
    static std::string OpenFolderDialog(const char* Title);

protected:
    static String m_strWorkingDirectory;
};

class StandardFile : public BasicFile
{
public:
    StandardFile(const FileOpenAttribs& OpenAttribs);
    virtual ~StandardFile() override;

    bool Read(IDataBlob* pData);

    bool Read(void* Data, size_t Size);

    bool Write(const void* Data, size_t Size);

    size_t GetSize();

    size_t GetPos();

    bool SetPos(size_t Offset, FilePosOrigin Origin);

protected:
    FILE* m_pFile = nullptr;
};
*/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPUDLIB_SPUDFILES_H
