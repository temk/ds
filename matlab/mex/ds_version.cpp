#include <mex.h>

#include <ds/version.h>

#include <stdexcept>

#include <ds/mx/utils.h>

using namespace ds;
using namespace std;

/**
 * ds_close(handle);
**/
void 
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 0 || nlhs != 1) {
		mexErrMsgTxt("expected 0 input and 1 output arguments");
	}

	char buff[128];
	sprintf(buff, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION);
	plhs[0] = mxCreateString(buff);
}
