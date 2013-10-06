#include <mex.h>
#include <matrix.h>

#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

#define MAX_META_LENGTH 1024
/**
 * ds_meta_add(handle, const char *key, const char *val, size_t col_optional );
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 3 || nrhs != 4 || !mxIsUint64(prhs[0]) || !mxIsChar(prhs[1])|| !mxIsChar(prhs[2])) {
        mexErrMsgTxt("expected 3 or 4 input arguments: handle, key, value, col (optional)");
	}

//    int64_t col = *(int64_t *)mxGetData(prhs[1]);

    char key[MAX_META_LENGTH];
    char val[MAX_META_LENGTH];

    mxGetString(prhs[1], &key[0], MAX_META_LENGTH);
    mxGetString(prhs[2], &val[0], MAX_META_LENGTH);

	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
        if (nrhs == 3) {
          stor.s ->tags().set(key, val);
        } else {
          size_t col = *(uint64_t *)mxGetData(prhs[0]);
          stor.s ->column_at(col).tags().set(key, val);
        }
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
