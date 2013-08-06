#include <mex.h>
#include <matrix.h>
#include <string.h>

#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include "utils.h"

using namespace ds;
using namespace std;

template<typename T>mxArray *
read_string(column &c, uint64_t *rows, size_t num) {
	const T **buff = new const T *[num];

	c.read(rows, num, buff);

	mxArray * data = mxCreateCellMatrix(num, 1);
	
	
	for (size_t k = 0; k < num; ++ k) {
		mwSize dim[2];		
		dim[0] = 1;
		dim[1] = str_length(buff[k]);
		
		mxArray *s = mxCreateCharArray(2, &dim[0]);
		str_copy(buff[k], mxGetChars(s), dim[1], false);
		mxSetCell(data, k, s);
	}
	delete [] buff;			
	
	return data;
}


/**
 * info = ds_read_index(handle, uint64 *cols, uint64 *rows);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 3 || !mxIsUint64(prhs[0])|| !mxIsUint64(prhs[1]) || !mxIsUint64(prhs[2])) {
		mexErrMsgTxt("expected 3 input argument: handle, uint64 *col, uint64  *rows");
	}

	if (nlhs != 1) {
		mexErrMsgTxt("expected 1 output argument: data cell array");
	}
	
	uint64_t * cols = (uint64_t *)mxGetData(prhs[1]);
	uint64_t * rows = (uint64_t *)mxGetData(prhs[2]);
	
	size_t col_num = mxGetNumberOfElements(prhs[1]);
	size_t row_num = mxGetNumberOfElements(prhs[2]);
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		plhs[0] = mxCreateCellMatrix(col_num, 1);
		
		for (size_t k = 0; k < col_num; ++ k) {
			column & c = stor.s ->column_at(cols[k]);
			
			//cout << "Read col " << cols[k] << ":" << c.name() << endl;
			
			mxArray *data = NULL;
			if (c.type() == DS_T_STRING8) {
				data = read_string<unsigned char>(c, rows, row_num);
			} else if (c.type() == DS_T_STRING16) {
				data = read_string<unsigned short>(c, rows, row_num);
			} else if (c.type() == DS_T_STRING32) {
				data = read_string<unsigned int>(c, rows, row_num);
			} else {
				data = mxCreateNumericMatrix(row_num, 1, type_to_class(c.type()), mxREAL);
				c.read(rows, row_num, mxGetData(data));
			}
			
			mxSetCell(plhs[0], k, data);
		}
		
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
