#include <mex.h>


#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

#define MAX_META_LENGTH 1024
/**
 * ds_meta_get(handle, size_t cols_optional );
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs < 1 || nrhs > 2 || !mxIsUint64(prhs[0])) {
        mexErrMsgTxt("expected 1 or 2 input arguments: handle, col (optional)");
    }

    if (nlhs < 2) {
        mexErrMsgTxt("expected 2 onput arguments: keys, values");
    }

	ds_handle stor;
  ds::meta *m;
	try {
    stor.h = *(uint64_t *)mxGetData(prhs[0]);
    if (nrhs == 1) {
      m = &stor.s ->tags();
    } else {
      size_t col = *(uint64_t *)mxGetData(prhs[1]);
      m = &stor.s ->column_at(col).tags();
    }

    vector<string> keys;
    m ->keys(keys);

    plhs[0] = mxCreateCellMatrix(keys.size(), 1);
    plhs[1] = mxCreateCellMatrix(keys.size(), 1);

    for (int i = 0; i < keys.size(); ++ i) {
      mxSetCell(plhs[0], i, mxCreateString(keys[i].c_str()));
      mxSetCell(plhs[1], i, mxCreateString(m ->get(keys[i]).c_str()));

    }
	} catch(const runtime_error &err) {
    mexErrMsgTxt(err.what());
	}
}
