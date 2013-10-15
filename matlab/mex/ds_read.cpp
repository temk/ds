#include <mex.h>
#include <matrix.h>

#include <ds/utils.h>
#include <ds/column.h>
#include <ds/storage.h>

#include <string.h>
#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

/**
 * info = ds_read(handle, uint64 *col, uint64 offs, uint64 num);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 4 || !mxIsUint64(prhs[0])|| !mxIsUint64(prhs[1])|| !mxIsUint64(prhs[2])) {
		mexErrMsgTxt("expected 4 input argument: handle, uint64 *col, uint64 offs, uint64 num");
	}
	
	if (nlhs != 1) {
		mexErrMsgTxt("expected 1 output argument: data cell array");
	}
	
	uint64_t * cols = (uint64_t *)mxGetData(prhs[1]);
	uint64_t offs = *(uint64_t *)mxGetData(prhs[2]);
	uint64_t tnum = *(uint64_t *)mxGetData(prhs[2]);
	size_t col_num = mxGetNumberOfElements(prhs[1]);
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		plhs[0] = mxCreateCellMatrix(col_num, 1);
		
		for (size_t k = 0; k < col_num; ++ k) {
			column & c = stor.s ->column_at(cols[k]);			
			uint64_t num = (tnum == 0) ? c.length() - offs : tnum;
			uint64_t width = c.width();
            
			mxArray *data = NULL;
			
			if (is_str(c.type())) {
				data = mxCreateCellMatrix(width, num);
				c.read(offs, num, data);
			} else {
				data = mxCreateNumericMatrix(width, num, type_to_class(c.type()), mxREAL);
				c.read(offs, num, mxGetData(data));
			}
			
			mxSetCell(plhs[0], k, data);
		}
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
