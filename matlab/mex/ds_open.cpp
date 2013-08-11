#include <mex.h>
#include <matrix.h>

#include <ds/utils.h>
#include <ds/storage.h>
#include <ds/column.h>

#include "utils.h"

#include <stdexcept>

using namespace ds;
using namespace std;

#define MAX_PATH_LEN 256
#define MAX_MODE_LEN 6


/**
 * handle = ds_open(const char *path, int mode, size_t buff_siz);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 3 || !mxIsChar(prhs[0]) || !mxIsChar(prhs[1]) || !mxIsUint64(prhs[2])) {
		mexErrMsgTxt("expected 3 input arguments: path (char []), mode (char []) and buf_siz(uint64)");
	}
	
	if (nlhs != 1) {
		mexErrMsgTxt("expected 1 output argument: handle");
	}
	
	char path[MAX_PATH_LEN];
	char mode[MAX_MODE_LEN];
	
	mxGetString(prhs[0], &path[0], MAX_PATH_LEN);
	mxGetString(prhs[1], &mode[0], MAX_MODE_LEN);
	
	uint64_t buff_siz = *(uint64_t *)mxGetData(prhs[2]);
	
	ds_handle stor;
	try {
		int int_mode = str_to_mode(mode);
		stor.s = new storage(path, int_mode, buff_siz);		
		for (size_t k = 0; k < stor.s ->cols(); ++ k) {
			column &col = stor.s ->column_at(k);
			if (is_str(col.type())) {
				col.set_string_accessor(get_string_accessor(col.type()));
			}
		}
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
	
	plhs[0] = mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
	*((uint64_t *)mxGetData(plhs[0])) = stor.h;
}
