
#if SPUDLIB_PLATFORM_WIN32

#include "spudfiles.h"
#include <ShObjIdl_core.h>
#include <Windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <string.h>

#if __cplusplus
extern "C" {
#endif

struct sfs_file_t {
#if _DEBUG
	const char *debug_name;
#endif
    HANDLE handle;
};

// --------------------------------------------------------------------------
// File open / close
// --------------------------------------------------------------------------

SPUDRESULT
sfs_file_open(SFS_FILE_OPEN_ATTRIBUTES open_attribs, sfs_file *out_file) {
    if (!open_attribs.str_file_path || open_attribs.str_file_path[0] == '\0')
        return SPUDRESULT_SFS_NULL_PATH;
    if (!out_file)
        return SPUDRESULT_NULL_OUTPUT_PARAMETER;

    DWORD access      = 0;
    DWORD disposition = 0;
    bool  append      = false;

    switch (open_attribs.access_mode) {
    case SFS_EFILE_ACCESS_MODE_READ:
        access      = GENERIC_READ;
        disposition = OPEN_EXISTING;
        break;
    case SFS_EFILE_ACCESS_MODE_OVERWRITE:
        access      = GENERIC_WRITE;
        disposition = CREATE_ALWAYS;
        break;
    case SFS_EFILE_ACCESS_MODE_APPEND:
        access      = GENERIC_WRITE;
        disposition = OPEN_ALWAYS;
        append      = true;
        break;
    case SFS_EFILE_ACCESS_MODE_READ_UPDATE:
        access      = GENERIC_READ | GENERIC_WRITE;
        disposition = OPEN_EXISTING;
        break;
    case SFS_EFILE_ACCESS_MODE_OVERWRITE_UPDATE:
        access      = GENERIC_READ | GENERIC_WRITE;
        disposition = CREATE_ALWAYS;
        break;
    case SFS_EFILE_ACCESS_MODE_APPEND_UPDATE:
        access      = GENERIC_READ | GENERIC_WRITE;
        disposition = OPEN_ALWAYS;
        append      = true;
        break;
    default:
        return SPUDRESULT_DESC_INVALID_PARAMETERS;
    }

    HANDLE h = CreateFileA(
        open_attribs.str_file_path,
        access,
        FILE_SHARE_READ,
        NULL,
        disposition,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (h == INVALID_HANDLE_VALUE)
        return SPUDRESULT_GENERAL_FAILURE;

    if (append) {
        if (SetFilePointer(h, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
            CloseHandle(h);
            return SPUDRESULT_GENERAL_FAILURE;
        }
    }

    struct sfs_file_t *f = (struct sfs_file_t *)malloc(sizeof(struct sfs_file_t));
    if (!f) {
        CloseHandle(h);
        return SPUDRESULT_OUT_OF_MEMORY;
    }
    f->handle = h;
    *out_file = f;
    return SPUD_SUCCESS;
}

SPUDRESULT sfs_file_release(sfs_file file) {
    if (!file) return SPUDRESULT_SFS_INVALID_FILE;
    CloseHandle(file->handle);
    free(file);
    return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File I/O
// --------------------------------------------------------------------------

SPUDRESULT sfs_file_read(sfs_file file, void *data, uint64_t size) {
    if (!file) return SPUDRESULT_SFS_INVALID_FILE;
    if (!data)  return SPUDRESULT_NULL_OUTPUT_PARAMETER;

    DWORD    bytes_read = 0;
    // ReadFile takes a DWORD count; loop for large reads
    uint64_t remaining  = size;
    uint8_t *dst        = (uint8_t *)data;

    while (remaining > 0) {
        DWORD chunk = remaining > 0xFFFFFFFFULL ? 0xFFFFFFFFUL : (DWORD)remaining;
        if (!ReadFile(file->handle, dst, chunk, &bytes_read, NULL))
            return SPUDRESULT_GENERAL_FAILURE;
        if (bytes_read == 0)
            break; // EOF
        dst       += bytes_read;
        remaining -= bytes_read;
    }
    return SPUD_SUCCESS;
}

SPUDRESULT sfs_file_write(sfs_file file, const void *data, uint64_t size) {
    if (!file) return SPUDRESULT_SFS_INVALID_FILE;
    if (!data)  return SPUDRESULT_NULL_OUTPUT_PARAMETER;

    DWORD          bytes_written = 0;
    uint64_t       remaining     = size;
    const uint8_t *src           = (const uint8_t *)data;

    while (remaining > 0) {
        DWORD chunk = remaining > 0xFFFFFFFFULL ? 0xFFFFFFFFUL : (DWORD)remaining;
        if (!WriteFile(file->handle, src, chunk, &bytes_written, NULL))
            return SPUDRESULT_GENERAL_FAILURE;
        src       += bytes_written;
        remaining -= bytes_written;
    }
    return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File position / size
// --------------------------------------------------------------------------

SPUDRESULT sfs_file_get_size(sfs_file file, uint64_t *out_size) {
    if (!file)     return SPUDRESULT_SFS_INVALID_FILE;
    if (!out_size) return SPUDRESULT_NULL_OUTPUT_PARAMETER;

    LARGE_INTEGER li;
    if (!GetFileSizeEx(file->handle, &li))
        return SPUDRESULT_GENERAL_FAILURE;

    *out_size = (uint64_t)li.QuadPart;
    return SPUD_SUCCESS;
}

SPUDRESULT sfs_file_get_pos(sfs_file file, uint64_t *out_pos) {
    if (!file)    return SPUDRESULT_SFS_INVALID_FILE;
    if (!out_pos) return SPUDRESULT_NULL_OUTPUT_PARAMETER;

    LARGE_INTEGER zero = {0};
    LARGE_INTEGER pos  = {0};
    if (!SetFilePointerEx(file->handle, zero, &pos, FILE_CURRENT))
        return SPUDRESULT_GENERAL_FAILURE;

    *out_pos = (uint64_t)pos.QuadPart;
    return SPUD_SUCCESS;
}

SPUDRESULT
sfs_file_set_pos(sfs_file file, uint64_t offset, SFS_FILE_POS_ORIGIN origin) {
    if (!file) return SPUDRESULT_SFS_INVALID_FILE;

    DWORD move_method;
    switch (origin) {
    case SFS_FILE_POS_ORIGIN_START:   move_method = FILE_BEGIN;   break;
    case SFS_FILE_POS_ORIGIN_CURRENT: move_method = FILE_CURRENT; break;
    case SFS_FILE_POS_ORIGIN_END:     move_method = FILE_END;     break;
    default: return SPUDRESULT_DESC_INVALID_PARAMETERS;
    }

    LARGE_INTEGER li;
    li.QuadPart = (LONGLONG)offset;
    if (!SetFilePointerEx(file->handle, li, NULL, move_method))
        return SPUDRESULT_GENERAL_FAILURE;

    return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File / directory queries
// --------------------------------------------------------------------------

bool sfs_file_exists(const char *file_path) {
    if (!file_path || file_path[0] == '\0') return false;
    DWORD attribs = GetFileAttributesA(file_path);
    return attribs != INVALID_FILE_ATTRIBUTES &&
           !(attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool sfs_directory_exists(const char *dir_path) {
    if (!dir_path || dir_path[0] == '\0') return false;
    DWORD attribs = GetFileAttributesA(dir_path);
    return attribs != INVALID_FILE_ATTRIBUTES &&
           (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

SPUDRESULT sfs_set_working_directory(const char *dir) {
    if (!dir || dir[0] == '\0') return SPUDRESULT_SFS_NULL_PATH;
    if (!SetCurrentDirectoryA(dir)) return SPUDRESULT_GENERAL_FAILURE;
    return SPUD_SUCCESS;
}

const char *sfs_get_working_directory() {
    static char buf[MAX_PATH];
    if (!GetCurrentDirectoryA(MAX_PATH, buf)) return NULL;
    return buf;
}

// --------------------------------------------------------------------------
// Directory creation
// --------------------------------------------------------------------------

SPUDRESULT sfs_create_directory(const char *path) {
    if (path == NULL || path[0] == '\0')
        return SPUDRESULT_SFS_NULL_PATH;

    char dir_path[MAX_PATH];
    strncpy(dir_path, path, MAX_PATH - 1);
    dir_path[MAX_PATH - 1] = '\0';

    // normalize slashes
    char *dp = dir_path;
    sfs_correct_slashes(&dp, SFS_SLASH);

    char *search   = dir_path;
    char *slash_pos;

    do {
        slash_pos = strchr(search, SFS_SLASH);

        char parent[MAX_PATH];
        if (slash_pos != NULL) {
            uint64_t len = (uint64_t)(slash_pos - dir_path);
            strncpy(parent, dir_path, len);
            parent[len] = '\0';
            search      = slash_pos + 1;
        } else {
            strncpy(parent, dir_path, MAX_PATH - 1);
            parent[MAX_PATH - 1] = '\0';
        }

        // skip drive letters like "C:"
        uint64_t plen = strlen(parent);
        if (plen > 0 && parent[plen - 1] == ':')
            continue;
        if (plen == 0)
            continue;

        if (!PathFileExistsA(parent)) {
            if (!CreateDirectoryA(parent, NULL)) {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS)
                    return SPUDRESULT_GENERAL_FAILURE;
                if ((GetFileAttributesA(parent) & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    return SPUDRESULT_GENERAL_FAILURE;
            }
        }
    } while (slash_pos != NULL);

    return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// File dialogs
// --------------------------------------------------------------------------

static DWORD FileDialogFlagsToOFNFlags(SFS_FILE_DIALOG_FLAGS FileDialogFlags) {
    DWORD OFNFlags = 0;
    while (FileDialogFlags != SFS_FILE_DIALOG_FLAG_NONE) {
        uint32_t Flag = FileDialogFlags & ~(FileDialogFlags - 1);
        switch (Flag) {
        case SFS_FILE_DIALOG_FLAG_DONT_ADD_TO_RECENT:
            OFNFlags |= OFN_DONTADDTORECENT;
            break;
        case SFS_FILE_DIALOG_FLAG_FILE_MUST_EXIST:
            OFNFlags |= OFN_FILEMUSTEXIST;
            break;
        case SFS_FILE_DIALOG_FLAG_NO_CHANGE_DIR:
            OFNFlags |= OFN_NOCHANGEDIR;
            break;
        case SFS_FILE_DIALOG_FLAG_OVERWRITE_PROMPT:
            OFNFlags |= OFN_OVERWRITEPROMPT;
            break;
        default:
            return 0;
        }
        FileDialogFlags &= ~Flag;
    }
    return OFNFlags;
}

const char *sfs_file_dialog(SFS_FILE_DIALOG_ATTRIBUTES attributes) {
    static char buffer[1024];

    OPENFILENAMEA ofn = {0};
    ofn.lStructSize   = sizeof(ofn);
    ofn.lpstrFilter   = attributes.filter;
    ofn.lpstrFile     = buffer;
    ofn.nMaxFile      = _countof(buffer);
    ofn.lpstrTitle    = attributes.title;
    ofn.Flags         = FileDialogFlagsToOFNFlags(attributes.flags);

    buffer[0] = '\0';

    if (attributes.type == SFS_FILE_DIALOG_TYPE_OPEN ? GetOpenFileNameA(&ofn)
                                                     : GetSaveFileNameA(&ofn)) {
        return buffer;
    }
    return NULL;
}

const char *sfs_open_folder_dialog(const char *title) {
#if defined(__MINGW64__) || defined(__MINGW32__)
    (void)title;
    return "";
#else
    static char result[MAX_PATH * 4];

    if (title == NULL)
        title = "Select Folder";

    IFileOpenDialog *pDialog = NULL;
    HRESULT hr = CoCreateInstance(
        &CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog,
        (void **)&pDialog);
    if (FAILED(hr))
        return "";

    DWORD dwOptions = 0;
    if (SUCCEEDED(pDialog->lpVtbl->GetOptions(pDialog, &dwOptions)))
        pDialog->lpVtbl->SetOptions(
            pDialog, dwOptions | FOS_PICKFOLDERS | FOS_NOCHANGEDIR);

    WCHAR wTitle[512];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, 512);
    pDialog->lpVtbl->SetTitle(pDialog, wTitle);

    hr = pDialog->lpVtbl->Show(pDialog, NULL);
    if (FAILED(hr)) {
        pDialog->lpVtbl->Release(pDialog);
        return "";
    }

    IShellItem *pItem = NULL;
    hr                = pDialog->lpVtbl->GetResult(pDialog, &pItem);
    pDialog->lpVtbl->Release(pDialog);
    if (FAILED(hr))
        return "";

    PWSTR pszFilePath = NULL;
    hr = pItem->lpVtbl->GetDisplayName(pItem, SIGDN_FILESYSPATH, &pszFilePath);
    pItem->lpVtbl->Release(pItem);
    if (FAILED(hr))
        return "";

    WideCharToMultiByte(
        CP_UTF8, 0, pszFilePath, -1, result, sizeof(result), NULL, NULL);
    CoTaskMemFree(pszFilePath);

    return result;
#endif
}

#if __cplusplus
}
#endif

#endif // SPUDLIB_PLATFORM_WIN32
