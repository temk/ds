#include <ds/column.h>
#include <ds/storage.h>
#include <ds/error.h>

using namespace ds;
using namespace std;

#define COL_LEN 100
#define DS_NAME "/tmp/storage"

void warning(const char *str) {
	cerr << "Warning: " << str << endl;
}

void info(const char *str) {
	cout << "Warning: " << str << endl;
}

template<typename T>void
add_column(storage &s, type_t type, const string &name, size_t len)
{
	T *val = new T[len];

	for (size_t k = 0; k < len; ++ k) {
		val[k] = T(k);
	}

	s.add(type, name).append(val, len);

	delete [] val;
}

template<typename T>void
add_column_str(storage &s, type_t type, const string &name, size_t len, T *VALS, int VAL_NUM)
{
	T *val = new T[len];

	for (size_t k = 0; k < len; ++ k) {
		val[k] = VALS[k % VAL_NUM];
	}

	s.add(type, DS_T_UINT32, name, DS_E_HOST).append(val, len);

	delete [] val;
}

static const char * str_vals [] = {"yes", "no", "may be" };
static const wchar_t * wstr_vals [] = {L"yes", L"no", L"may be" };

void fill(storage &stor)
{

	add_column<bool>(stor, DS_T_BOOL, "var_bool", COL_LEN);

	add_column<int8_t>(stor,  DS_T_INT8,  "var_int8", COL_LEN);
	add_column<int16_t>(stor, DS_T_INT16, "var_int16", COL_LEN);
	add_column<int32_t>(stor, DS_T_INT32, "var_int32", COL_LEN);
	add_column<int64_t>(stor, DS_T_INT64, "var_int64", COL_LEN);

	add_column<uint8_t>(stor,  DS_T_UINT8,  "var_uint8", COL_LEN);
	add_column<uint16_t>(stor, DS_T_UINT16, "var_uint16", COL_LEN);
	add_column<uint32_t>(stor, DS_T_UINT32, "var_uint32", COL_LEN);
	add_column<uint64_t>(stor, DS_T_UINT64, "var_uint64", COL_LEN);

	add_column<float32_t>(stor, DS_T_FLOAT32, "var_float32", COL_LEN);
	add_column<float64_t>(stor, DS_T_FLOAT64, "var_float64", COL_LEN);

	add_column_str<const char *>(stor, DS_T_STRING8, "var_str", COL_LEN, &str_vals[0], sizeof(str_vals)/sizeof(str_vals[0]));
	add_column_str<const wchar_t *>(stor, DS_T_STRING32, "var_wstr", COL_LEN, &wstr_vals[0], sizeof(wstr_vals)/sizeof(wstr_vals[0]));

}

void create_stor()
{
	storage stor;
	stor.warn.set(&warning);
	stor.info.set(&info);

	stor.open(DS_NAME, DS_O_DEFAULT | DS_O_TRUNC);
	fill(stor);
	stor.close();
}

#define bool_t bool
#define READ_VAR(T) T ## _t *var_ ## T = new T ## _t [stor["var_" #T].length()]; \
	stor["var_" #T].read(0, stor["var_" #T].length(), var_ ## T);

void read_stor()
{
	storage stor;
	stor.warn.set(&warning);
	stor.info.set(&info);

	stor.open(DS_NAME, DS_O_READ);

	READ_VAR(bool)

	READ_VAR(int8)
	READ_VAR(int16)
	READ_VAR(int32)
	READ_VAR(int64)

	READ_VAR(uint8)
	READ_VAR(uint16)
	READ_VAR(uint32)
	READ_VAR(uint64)

	READ_VAR(float32)
	READ_VAR(float64)

	size_t len = stor["var_str"].length();
	const char ** var_str = new const char *[len];
	stor["var_str"].read(0, len, var_str);
	cout << "+++++++++++++++++"<< endl;
	for (int k = 0; k < 10; ++ k) {
		cout << string(var_str[k]) << endl;
	}
	cout << "----------------" << endl;

	size_t wlen = stor["var_wstr"].length();
	const wchar_t ** var_wstr = new const wchar_t *[wlen];
	stor["var_wstr"].read(0, len, var_wstr);

	cout << "+++++++++++++++++"<< endl;
	for (int k = 0; k < 10; ++ k) {
		wcout << wstring(var_wstr[k]) << endl;
	}
	cout << "----------------" << endl;
}


int
main1(int argc, char **argv)
{
	try {
		create_stor();
		read_stor();
	} catch(const exception &ex) {
		cerr << "Exception: " << ex.what() << endl;
	}
	return 0;
}

int
main(int argc, char **argv)
{
  storage stor;
  stor.warn.set(&warning);
  stor.info.set(&info);

  stor.open(DS_NAME, DS_O_DEFAULT | DS_O_TRUNC | DS_O_WRITE | DS_O_SAFE);
  column & c = stor.add(DS_T_STRING8, DS_T_UINT32, "x", 3);
  const char * data[] = {"1","2","3","4","5","6","7","8","9","10","11","12"};
  c.append(data, 4);
  stor.flush();

  const char * d1[6];
  const char * d2[6];
  int idx[] = {1,3};
  c.read(1, 2, d1);
  c.read(idx, sizeof(int), 2, d2);
  cout << "=== d1 === " << endl;
  for (int k = 0; k < 2; ++ k) {
    for (int s = 0; s < 3; ++ s) {
      cout << d1[k * 3 + s] << " ";
    }
    cout << endl;
  }

  cout << "=== d2 === " << endl;
  for (int k = 0; k < 2; ++ k) {
    for (int s = 0; s < 3; ++ s) {
      cout << d2[k * 3 + s] << " ";
    }
    cout << endl;
  }

  stor.flush();
  stor.close();
}
