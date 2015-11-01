#include <mex.h>


#include <ds/utils.h>
#include <ds/column.h>
#include <ds/storage.h>

#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

#define MAX_TYPE_LEN 12
#define MAX_NAME_LEN 1024
#define MAX_ENDIAN_LEN 12



/**
 * ds_add(handle, int type, const char *name, const char *endian, int64 idx, int32 siz, uint64 width);
**/
void
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 7 || !mxIsUint64(prhs[0]) || !mxIsChar(prhs[1]) || !mxIsChar(prhs[2]) || !mxIsChar(prhs[3]) ||
                     !mxIsInt64(prhs[4])  || !mxIsChar(prhs[5]) || !mxIsUint64(prhs[6]) ) {
		mexErrMsgTxt("expected 7 input argument: handle, string type, string name, string endian, int64 idx, int32 siz, width");
	}

	char str_type[MAX_TYPE_LEN];
	char str_name[MAX_NAME_LEN];
	char str_endian[MAX_ENDIAN_LEN];

	mxGetString(prhs[1], &str_type[0], MAX_TYPE_LEN);
	mxGetString(prhs[2], &str_name[0], MAX_NAME_LEN);
	mxGetString(prhs[3], &str_endian[0], MAX_ENDIAN_LEN);

    uint64_t width = *(uint64_t *)mxGetData(prhs[6]);
	int64_t ix = *(int64_t *)mxGetData(prhs[4]);

	ds_handle stor;
	try {
		stor.h = *(uint64_t *)mxGetData(prhs[0]);
		type_t in_type = str_to_type(&str_type[0]);
		if (!is_str(in_type)) {
            stor.s ->add(in_type, &str_name[0], width, str_to_endian(&str_endian[0]), ix);
		} else {
			mxGetString(prhs[5], &str_type[0], MAX_TYPE_LEN);
            column &col = stor.s ->add(in_type, str_to_type(&str_type[0]), &str_name[0], width, str_to_endian(&str_endian[0]), ix);
			col.set_string_accessor(get_string_accessor(in_type));
		}
	} catch(const runtime_error &err) {
		mexErrMsgTxt(err.what());
	}
}
