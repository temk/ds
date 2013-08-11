#include "dsjni.h"

#include <ds/types.h>
#include <ds/utils.h>
#include <ds/lookup.h>
#include <ds/storage.h>
#include <ds/column.h>

#include <algorithm>

using namespace ds;
using namespace std;

typedef pair<JNIEnv *, jobjectArray> str_pair_t;

// ======================================================================================
template<typename T>
class java_string_accessor : public string_accessor {
private:
	JNIEnv *env_;
	
public:
	java_string_accessor(JNIEnv *env) : env_(env) {}
	
	void get(size_t k, const void *data, string_container &s) const {
		jobjectArray array = (jobjectArray)data;
		
		jstring str = (jstring )env_ ->GetObjectArrayElement(array, k);
		s.siz = sizeof(jchar);
		s.len = env_ ->GetStringLength(str);
		s.str = env_ ->GetStringChars( str, 0);
	}
	
	void set(size_t k, const string_container &s, void *array ) const {
		cout << 	"len: " << s.len << endl;
		
		jstring str = NULL;
		if (s.siz == sizeof(jchar)) {
			str = env_ ->NewString((jchar *)s.str, s.len);
		} else {
			jchar *tmp = new jchar[s.len];
			str_copy<T, jchar>((const T*)s.str, (jchar *)tmp, s.len);
			str = env_ ->NewString(tmp, s.len);
			delete [] tmp;
		}
		env_ ->SetObjectArrayElement((jobjectArray)array, k, str);
	}
};


string_accessor * get_string_accessor(JNIEnv * env, type_t type) {
	switch(type) {
		case DS_T_STRING8:  return new java_string_accessor<char>(env);
		case DS_T_STRING16: return new java_string_accessor<short>(env);
		case DS_T_STRING32: return new java_string_accessor<int>(env);
		default: return NULL;
	}
}

// ======================================================================================
static void jni_to_string(JNIEnv *env, jstring jstr, string &result) {
	const char *s = env->GetStringUTFChars(jstr, 0);
	result = s;
	env->ReleaseStringUTFChars(jstr, s);
}

static jobject endian_to_jni(JNIEnv *env, endian_t endian) {
	jobject result = NULL;
	string name = "invalid";
	
	jclass clazz = env->FindClass( "java/nio/ByteOrder" );
	jfieldID fid = 0;
	switch(endian) {
	case DS_E_BIG:
		fid = env ->GetStaticFieldID(clazz, "BIG_ENDIAN", "Ljava/nio/ByteOrder;");
		break;
		
	case DS_E_LITTLE:
		fid = env ->GetStaticFieldID(clazz, "LITTLE_ENDIAN", "Ljava/nio/ByteOrder;");
		break;
		
	case DS_E_INVALID:
	default:
		break;
	}
	
	result = env ->GetStaticObjectField(clazz, fid);
	return result;
}

#define CASE_TYPE(t) case DS_T_ ## t: name = #t; break;

static jobject type_to_jni(JNIEnv *env, type_t type) {
	jobject result = NULL;
	string sig, name;

	jclass clazz = env->FindClass( "org/temk/ds/Type" );
	
	switch(type) {
	CASE_TYPE(INVALID);
	CASE_TYPE(BOOL);
	CASE_TYPE(INT8);
	CASE_TYPE(INT16);
	CASE_TYPE(INT32);
	CASE_TYPE(INT64);
	CASE_TYPE(UINT8);
	CASE_TYPE(UINT16);
	CASE_TYPE(UINT32);
	CASE_TYPE(UINT64);
	CASE_TYPE(FLOAT32);
	CASE_TYPE(FLOAT64);
	CASE_TYPE(STRING8);
	CASE_TYPE(STRING16);
	CASE_TYPE(STRING32);
	};
	
	jfieldID fid = env ->GetStaticFieldID(clazz, name.c_str(), "Lorg/temk/ds/Type;");
	result =  env ->GetStaticObjectField(clazz, fid);
	
	return result;
}

#define CHECK_TYPE(s, t) if (s == #t) { return DS_T_ ## t; }

static type_t jni_to_type(JNIEnv *env, jobject value) {
	jclass clazz = env->FindClass( "org/temk/ds/Type" );
	jmethodID meth = env->GetMethodID(clazz, "name", "()Ljava/lang/String;");
	
	string name;	
	jni_to_string(env, (jstring)env->CallObjectMethod(value, meth), name);
	
    CHECK_TYPE(name, BOOL);
    CHECK_TYPE(name, INT8);
    CHECK_TYPE(name, INT16);
    CHECK_TYPE(name, INT32);
    CHECK_TYPE(name, INT64);
    CHECK_TYPE(name, UINT8);
    CHECK_TYPE(name, UINT16);
    CHECK_TYPE(name, UINT32);
    CHECK_TYPE(name, UINT64);
    CHECK_TYPE(name, FLOAT32);
    CHECK_TYPE(name, FLOAT64);
    CHECK_TYPE(name, STRING8);
    CHECK_TYPE(name, STRING16);
    CHECK_TYPE(name, STRING32);
	
	return DS_T_INVALID;
}

static endian_t 
jni_to_endian(JNIEnv *env, jobject value) {
	jclass clazz = env->FindClass( "java/nio/ByteOrder" );
	jmethodID meth = env->GetMethodID(clazz, "toString", "()Ljava/lang/String;");

	string name;	
	jni_to_string(env, (jstring)env->CallObjectMethod(value, meth), name);
	
	if (name == "BIG_ENDIAN") {
		return DS_E_BIG;
	} else if (name == "LITTLE_ENDIAN") {
		return DS_E_LITTLE;
	} else {
		return DS_E_INVALID;
	}
}

static void jni_throw(JNIEnv *env, const string &msg) {
	jclass clazz = env->FindClass( "java/lang/RuntimeException" );
	env->ThrowNew( clazz, msg.c_str() );
}

static int str_to_mode(const char *str) {
	int mode = 0;
	for (; *str; ++ str) {
		switch (*str) {
		case 'r':  
			mode |= DS_O_READ; 
			break;
			
		case 'w':  
			mode |= DS_O_WRITE; 
			break;
			
		case 'c':  
			mode |= DS_O_CREATE; 
			break;
			
		case 't':  
			mode |= DS_O_TRUNC; 
			break;
		} 
	}
	
	return mode;
}


template<typename T>
static void set_handle(JNIEnv *env, jobject self, T *handle) {
	jclass clazz = env->GetObjectClass(self);
 
   jfieldID fid = env->GetFieldID(clazz, "handle", "J");
   env->SetLongField(self, fid, jlong(handle));	
}

template<typename T>
static void get_handle(JNIEnv *env, jobject self, T *&handle) {
	jclass clazz = env->GetObjectClass(self);
 
    jfieldID fid = env->GetFieldID(clazz, "handle", "J");
    jlong value = env->GetLongField(self, fid);	
    handle = reinterpret_cast<T *>(value);
}

// ========================================================================================================

JNIEXPORT void JNICALL 
Java_org_temk_ds_DataStorage_open(JNIEnv *env, jobject self, jstring jpath, jstring jmode, jlong buff_siz) {
	string path, mode;	
	jni_to_string(env, jpath, path);
	jni_to_string(env, jmode, mode);
	
	try {
		storage *stor = new storage(path, str_to_mode(mode.c_str()), buff_siz);		
		set_handle(env, self, stor);
		
		jclass clazz = env->FindClass( "org/temk/ds/Column" );
		jmethodID meth = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;JLorg/temk/ds/DataStorage;)V");
		for (size_t k = 0; k < stor ->cols(); ++ k) {
			column *col = &stor ->column_at(k);
			if (is_str(col ->type())) {
				col ->set_string_accessor(get_string_accessor(env, col ->type()));				
			}
			
			jstring jname = env ->NewStringUTF(col ->name().c_str());
			env->NewObject(clazz, meth, jname, jlong(col), self);			
		}
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}

JNIEXPORT void JNICALL 
Java_org_temk_ds_DataStorage_close(JNIEnv *env, jobject self) {
	storage *stor;
	try {		
		get_handle(env, self, stor);
		if (stor != NULL) {
			for (size_t k = 0; k < stor ->cols(); ++ k) {
				column *col = &stor ->column_at(k);
				if (is_str(col ->type())) {
					//string_accessor * acc = col ->get_string_accessor();				
					//delete acc;
				}
			}
			stor ->close();
		}
		delete stor;
		set_handle(env, self, (storage *)NULL);
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}

JNIEXPORT jlong JNICALL 
Java_org_temk_ds_DataStorage_getColumnNumber(JNIEnv *env, jobject self) {
	storage *stor;
	jlong result = -1;
	try {		
		get_handle(env, self, stor);
		result = stor ->cols();
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	return result;
}

JNIEXPORT jlong JNICALL 
Java_org_temk_ds_DataStorage_getColumnByIndex(JNIEnv *env, jobject self, jlong index) {
	storage *stor;
	jlong result = 0;
	try {		
		get_handle(env, self, stor);
		result = jlong( &stor ->column_at(index) );
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	return result;
}

JNIEXPORT jlong
JNICALL Java_org_temk_ds_DataStorage_getColumnByName(JNIEnv *env, jobject self, jstring jname) { 
	storage *stor;
	jlong result = 0;
	try {		
		string name;
		jni_to_string(env, jname, name);
		
		get_handle(env, self, stor);
		
		result = jlong(& stor ->column_at(name) );
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	return result;
}

JNIEXPORT jobject JNICALL 
Java_org_temk_ds_DataStorage_addColumn(JNIEnv *env, jobject self, jobject jtype, jobject jext_type, jstring jname, jobject jendian, jlong index) {
	type_t type = jni_to_type(env, jtype);
	type_t ext_type = jni_to_type(env, jext_type);
	endian_t endian = jni_to_endian(env, jendian);
	
	string name;
	jni_to_string(env, jname, name); 
	
	storage *stor = NULL;
	column  *col = NULL;
	jobject result = NULL; 
	try {		
		get_handle(env, self, stor);
		
		if (is_str(type)) {
			col = &stor ->add(type, ext_type, name, endian, index);
			col ->set_string_accessor(get_string_accessor(env, type));
		} else {
			col = &stor ->add(type, name, endian, index);			
		}
		
		jclass clazz = env->FindClass( "org/temk/ds/Column" );
		jmethodID meth = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;JLorg/temk/ds/DataStorage;)V");
		result = env->NewObject(clazz, meth, jname, jlong(col), self);
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	
	return result;
}

JNIEXPORT void JNICALL 
Java_org_temk_ds_DataStorage_flush(JNIEnv *env, jobject self) {
	storage *stor;
	try {		
		get_handle(env, self, stor);
		stor ->flush();
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}


// ========================================================================================================
JNIEXPORT void 
JNICALL Java_org_temk_ds_Column_remove(JNIEnv *env, jobject self) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);
		col ->remove();
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}

JNIEXPORT void JNICALL 
Java_org_temk_ds_Column_flush(JNIEnv *env, jobject self) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);
		col ->flush();
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}

JNIEXPORT void
JNICALL Java_org_temk_ds_Column_truncate(JNIEnv *env, jobject self, jlong new_length) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);
		col ->truncate(new_length);
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}

#define GET_ARRAY_DATA(T, t, u) case DS_T_ ## T : data = env ->Get ## t ## ArrayElements(static_cast<j ## u ## Array>(array), 0); break

JNIEXPORT void JNICALL 
Java_org_temk_ds_Column_append(JNIEnv *env, jobject self, jobject array) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);	

		jsize len = env ->GetArrayLength(static_cast<jarray>(array));
		void *data = NULL;
		
		switch(col ->type()) {
		GET_ARRAY_DATA(BOOL,    Boolean, boolean);
		GET_ARRAY_DATA(INT8,    Byte,    byte);
		GET_ARRAY_DATA(INT16,   Short,   short);
		GET_ARRAY_DATA(INT32,   Int,     int);
		GET_ARRAY_DATA(INT64,   Long,    long);
		GET_ARRAY_DATA(UINT8,   Byte,    byte);
		GET_ARRAY_DATA(UINT16,  Short,   short);
		GET_ARRAY_DATA(UINT32,  Int,     int);
		GET_ARRAY_DATA(UINT64,  Long,    long);
		GET_ARRAY_DATA(FLOAT32, Float,   float);
		GET_ARRAY_DATA(FLOAT64, Double,  double);
		
		case DS_T_STRING8:
		case DS_T_STRING16:
		case DS_T_STRING32:
			data = array;
			break;
			
		default: 
			break;
		}
		col ->append(data, len);
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}	
}

JNIEXPORT void JNICALL 
Java_org_temk_ds_Column_read(JNIEnv *env, jobject self, jlong offset, jobject indexes, jobject data) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);	
		
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}	
	
	jni_throw(env, "READ not implemented");
}

JNIEXPORT jlong JNICALL 
Java_org_temk_ds_Column_getIndex(JNIEnv *env, jobject self) {
	column *col = NULL;
	jlong result = -1;
	try {		
		get_handle(env, self, col);
		result = col ->index();
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}	
	
	return result;
}

JNIEXPORT jlong 
JNICALL Java_org_temk_ds_Column_getLength(JNIEnv *env, jobject self) {
	column *col = NULL;
	jlong result = -1;
	try {		
		get_handle(env, self, col);
		result = col ->length();
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}	
	
	return result;
}

JNIEXPORT jobject JNICALL 
Java_org_temk_ds_Column_getExtType(JNIEnv *env, jobject self) {
	column *col = NULL;
	jobject result = NULL;
	try {		
		get_handle(env, self, col);
		result = type_to_jni(env, col ->ext_type());
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	return result;
}

JNIEXPORT jobject JNICALL 
Java_org_temk_ds_Column_getByteOrder(JNIEnv *env, jobject self) {
	column *col = NULL;
	jobject result = NULL;
	try {		
		get_handle(env, self, col);
		result = endian_to_jni(env, col ->endian());
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	return result;
}

JNIEXPORT jobject JNICALL 
Java_org_temk_ds_Column_getType(JNIEnv *env, jobject self) {
	column *col = NULL;
	jobject result = NULL;
	try {		
		get_handle(env, self, col);
		result = type_to_jni(env, col ->type());
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
	return result;
}
