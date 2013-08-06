#include <mex.h>
#include <matrix.h>

#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include "utils.h"

using namespace ds;
using namespace std;

#define MAX_TYPE_LEN 12
#define MAX_NAME_LEN 128
#define MAX_ENDIAN_LEN 12



/**
 * ds_add(handle, int type, const char *name, const char *endian, int64 idx, int32 siz);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 6 || !mxIsUint64(prhs[0]) || !mxIsChar(prhs[1]) || !mxIsChar(prhs[2]) || !mxIsChar(prhs[3]) || !mxIsInt64(prhs[4]) || !mxIsInt32(prhs[5])) {
		mexErrMsgTxt("expected 6 input argument: handle, string type, string name, string endian, int64 idx, int32 siz");
	}
	
	char str_type[MAX_TYPE_LEN];
	char str_name[MAX_NAME_LEN];
	char str_endian[MAX_ENDIAN_LEN];
	
	mxGetString(prhs[1], &str_type[0], MAX_TYPE_LEN);
	mxGetString(prhs[2], &str_name[0], MAX_NAME_LEN);
	mxGetString(prhs[3], &str_endian[0], MAX_ENDIAN_LEN);
	
	int64_t ix = *(int64_t *)mxGetData(prhs[4]);
	int32_t sz = *(int32_t *)mxGetData(prhs[5]);
	
	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		stor.s ->add(str_to_type(&str_type[0]), &str_name[0], str_to_endian(&str_endian[0]), ix, sz);
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
