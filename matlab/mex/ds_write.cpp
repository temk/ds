#include <mex.h>
#include <matrix.h>

#include <ds/utils.h>
#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

/**
 * ds_write(handle, col, data, num);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 3 || !mxIsUint64(prhs[0]) || !mxIsUint64(prhs[1]) || !mxIsCell(prhs[2])) {
		mexErrMsgTxt("expected 3 input argument: handle, uint64 *col, cell array data");
	}
	
	uint64_t *cols = (uint64_t *)mxGetData(prhs[1]);
	size_t col_num = mxGetNumberOfElements(prhs[1]);
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		for (size_t k = 0; k < col_num; ++ k) {
			column &c = stor.s ->column_at(cols[k]);
			
			
			mxArray *data = mxGetCell(prhs[2], k);
			size_t len = mxGetNumberOfElements(data);

			if (is_str(c.type())) {
				c.append(data, len);
			} else {
				c.append(mxGetData(data), len);				
			}
			
		}
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
