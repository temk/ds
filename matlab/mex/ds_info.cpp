#include <mex.h>
#include <matrix.h>

#include <ds/utils.h>
#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include "utils.h"

using namespace ds;
using namespace std;

static const char * FIELDS[] = {"name", "length", "type", "size", "endian" };

/**
 * info = ds_info(handle);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 1 || !mxIsUint64(prhs[0])) {
		mexErrMsgTxt("expected 1 input argument: handle");
	}
	
	if (nlhs != 1) {
		mexErrMsgTxt("expected 1 output argument: struct array");
	}
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		
			
		size_t n = stor.s ->cols();
		mwSize sz = n;
		plhs[0] = mxCreateStructArray(1, &sz, sizeof(FIELDS)/sizeof(FIELDS[0]), FIELDS);
		
		for (size_t k = 0; k < n; ++ k) {
			const column &c = stor.s ->column_at(k);

			 mxSetField(plhs[0], k, FIELDS[0], mxCreateString(c.name().c_str()));
			 mxSetField(plhs[0], k, FIELDS[1], mxCreateDoubleScalar(c.length()));
			 mxSetField(plhs[0], k, FIELDS[2], mxCreateString(type_to_str(c.type())));
			 mxSetField(plhs[0], k, FIELDS[3], mxCreateDoubleScalar(size_of(c.ext_type())));
			 mxSetField(plhs[0], k, FIELDS[4], mxCreateString(endian_to_str(c.endian())));
		}

			
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
