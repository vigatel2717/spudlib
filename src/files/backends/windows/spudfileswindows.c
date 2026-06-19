
#if SPUDLIB_PLATFORM_WIN32

#include "spudfiles.h"
#include <ShObjIdl_core.h>
#include <Windows.h>
#include <shlwapi.h>
#include <string.h>

#if __cplusplus
extern "C" {
#endif

SPUDRESULT sfs_create_directory(const char *path) {
    if (path == NULL || path[0] == '\0')
        return SPUDRESULT_SFS_NULL_PATH;

    char dir_path[MAX_PATH];
    strncpy(dir_path, path, MAX_PATH - 1);
    dir_path[MAX_PATH - 1] = '\0';

    /* normalize slashes */
    sfs_correct_slashes(&dir_path, '\0');

    const char slash = SFS_SLASH;
    char *search     = path;
    char *slash_pos;

    do {
        slash_pos = strchr(search, slash);

        char parent[MAX_PATH];
        if (slash_pos != NULL) {
            uint64_t len = (uint64_t)(slash_pos - dir_path);
            strncpy(parent, dir_path, len);
            parent[len] = '\0';
            search = slash_pos + 1;
        } else {
            strncpy(parent, dir_path, MAX_PATH - 1);
            parent[MAX_PATH - 1] = '\0';
        }

        /* skip drive letters like "C:" */
        uint64_t plen = strlen(parent);
        if (plen > 0 && parent[plen - 1] == ':')
            continue;

        if (!PathFileExistsA(parent)) {
            if (!CreateDirectoryA(parent, NULL)) {
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                    return false;
                if ((GetFileAttributesA(parent) & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    return false;
            }
        }
    } while (slash_pos != NULL);

    return true;
}
static DWORD FileDialogFlagsToOFNFlags(SFS_FILE_DIALOG_FLAGS FileDialogFlags) {
	DWORD OFNFlags = 0;
	while (FileDialogFlags != SFS_FILE_DIALOG_FLAG_NONE) {
		uint32_t Flag =
		    FileDialogFlags & ~(FileDialogFlags - 1);
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
			//UNEXPECTED("Unknown file dialog flag (", Flag, ")");
            return 0;
		}
		FileDialogFlags &= ~Flag;
	}
	return OFNFlags;
}
const char *sfs_file_dialog(SFS_FILE_DIALOG_ATTRIBUTES attributes) {
	OPENFILENAMEA ofn = {0};

	char buffer[1024];
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = attributes.filter;
	ofn.lpstrFile   = buffer;
	ofn.nMaxFile    = _countof(buffer);
	ofn.lpstrTitle  = attributes.title;
	ofn.Flags       = FileDialogFlagsToOFNFlags(attributes.flags);

	const char *filename;
	if (attributes.type == SFS_FILE_DIALOG_TYPE_OPEN ? GetOpenFileNameA(&ofn)
	                                                 : GetSaveFileNameA(&ofn)) {
		filename = buffer;
	}
	return filename;
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
	HRESULT hr               = CoCreateInstance(
	    &CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog,
	    (void **)&pDialog);
	if (FAILED(hr))
		return "";

	DWORD dwOptions = 0;
	if (SUCCEEDED(pDialog->lpVtbl->GetOptions(pDialog, &dwOptions)))
		pDialog->lpVtbl->SetOptions(
		    pDialog, dwOptions | FOS_PICKFOLDERS | FOS_NOCHANGEDIR);

	/* title: narrow -> wide */
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

	/* wide -> narrow */
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
