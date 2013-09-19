#include <mex.h>
#include <matrix.h>

#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

/**
 * ds_remove(handle, size_t index);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 2 || !mxIsUint64(prhs[0]) || !mxIsUint64(prhs[1])) {
		mexErrMsgTxt("expected 2 input arguments: handle, column indexes");
	}

	uint64_t * col = (uint64_t *)mxGetData(prhs[1]);
	size_t num = mxGetNumberOfElements(prhs[1]);
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		for (size_t k = 0; k < num; ++ k) {
			stor.s ->column_at(col[k]).remove();		
		}
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
