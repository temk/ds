#include <mex.h>
#include <matrix.h>

#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include "utils.h"

using namespace ds;
using namespace std;

/**
 * ds_trunc(handle, size_t index, size_t len);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 3 || !mxIsUint64(prhs[0]) || !mxIsUint64(prhs[1])|| !mxIsUint64(prhs[2])) {
		mexErrMsgTxt("expected 3 input arguments: handle, column index, new length");
	}

	uint64_t * col = (uint64_t *)mxGetData(prhs[1]);
	uint64_t * len = (uint64_t *)mxGetData(prhs[2]);
	size_t num = mxGetNumberOfElements(prhs[2]);
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		for (size_t k = 0; k < num; ++ k) {
			stor.s ->column_at(col[k]).truncate(len[k]);		
		}
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
