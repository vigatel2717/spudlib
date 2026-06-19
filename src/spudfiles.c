
#include "spudfiles.h"

//SPUDRESULT sfs_file_read(sfs_file file, spudblob data) {
//    return SPUDRESULT_NOT_IMPLEMENTED_YET;
//}
SPUDRESULT sfs_file_read(sfs_file file, void *data, uint64_t size) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
SPUDRESULT sfs_file_write(sfs_file file, const void *data, uint64_t size) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET; 
}
SPUDRESULT sfs_file_get_size(sfs_file file, uint64_t *out_size) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
SPUDRESULT sfs_file_get_pos(sfs_file file, uint64_t *out_pos) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
SPUDRESULT
sfs_file_set_pos(sfs_file file, uint64_t offset, SFS_FILE_POS_ORIGIN origin) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}

SPUDRESULT
sfs_file_open(SFS_FILE_OPEN_ATTRIBUTES open_attribs, sfs_file *out_file) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
SPUDRESULT sfs_file_release(sfs_file file) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
bool sfs_file_exists(const char *file_path) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
SPUDRESULT sfs_set_working_directory(const char *dir) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}
const char *sfs_get_working_directory() {
    return NULL;
}
bool sfs_is_slash(char c) {
    return SPUDRESULT_NOT_IMPLEMENTED_YET;
}

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