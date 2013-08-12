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
		s.str = env_ ->GetStringCritical(str, 0);
		
		env_ ->ReleaseStringCritical(str, (jchar *)s.str);
	}
	
	void set(size_t k, const string_container &s, void *array ) const {
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

static void 
jni_read(column *col, JNIEnv *env, jarray data, jlong offset, jlong num) {

	if (is_str(col->type())) {
		col ->read(offset, num, data);
	} else {
		void *ptr = env ->GetPrimitiveArrayCritical(data, 0);
		col ->read(offset, num, ptr);
		env ->ReleasePrimitiveArrayCritical(data, ptr, 0);
	}
	
}

static void 
jni_read(column *col, JNIEnv *env, jarray data, jarray indexes, int idx_siz) {
	size_t num = env ->GetArrayLength(data);
	void *idx = env ->GetPrimitiveArrayCritical(indexes, 0);

	if (is_str(col->type())) {
		col ->read(idx, idx_siz, num, data);
	} else {
		void *ptr = env ->GetPrimitiveArrayCritical(data, 0);
		col ->read(idx, idx_siz, num, ptr);
		env ->ReleasePrimitiveArrayCritical(data, ptr, 0);
	}
	
	env ->ReleasePrimitiveArrayCritical(indexes, idx, 0);
}

static void 
jni_write(column *col, JNIEnv *env, jarray data) {
	size_t num = env ->GetArrayLength(data);
	
	if (is_str(col->type())) {
		col ->append(data, num);
	} else {
		void *ptr = env ->GetPrimitiveArrayCritical(data, 0);
		col ->append(ptr, num);
		env ->ReleasePrimitiveArrayCritical(data, ptr, 0);
	}
}

JNIEXPORT void 
JNICALL Java_org_temk_ds_Column_read(JNIEnv *env, jobject self, jobject data, jobject indexes, jint index_siz, jlong offset, jlong num) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);
		if (indexes == NULL) {
			jni_read(col, env, static_cast<jarray>(data), offset, num);			
		} else {
			jni_read(col, env, static_cast<jarray>(data), static_cast<jarray>(indexes), index_siz);			
		}
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}

JNIEXPORT void 
JNICALL Java_org_temk_ds_Column_write(JNIEnv *env, jobject self, jobject data) {
	column *col = NULL;
	try {		
		get_handle(env, self, col);
		jni_write(col, env, static_cast<jarray>(data));
	} catch(const exception &ex) {
		jni_throw(env, ex.what());
	}
}