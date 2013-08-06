#include <mex.h>
#include <matrix.h>

#include <ds/storage.h>

#include <stdexcept>

#include "utils.h"

using namespace ds;
using namespace std;

/**
 * ds_close(handle);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 1 || !mxIsUint64(prhs[0])) {
		mexErrMsgTxt("expected 1 input argument: handle");
	}
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		stor.s ->close();
		delete stor.s;		
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
