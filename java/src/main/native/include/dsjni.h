#ifndef __DS_JNI_H__
#define __DS_JNI_H__

#include <jni.h>

#include <ds/storage.h>

extern "C" {
	JNIEXPORT void JNICALL Java_org_temk_ds_DataStorage_open(JNIEnv *env, jobject self, jstring path, jstring mode, jlong buff_siz);
	JNIEXPORT void JNICALL Java_org_temk_ds_DataStorage_close(JNIEnv *env, jobject self);
	JNIEXPORT jlong JNICALL Java_org_temk_ds_DataStorage_getColumnNumber(JNIEnv *env, jobject self);
	JNIEXPORT jlong JNICALL Java_org_temk_ds_DataStorage_getColumnByIndex(JNIEnv *env, jobject self, jlong index);
	JNIEXPORT jlong JNICALL Java_org_temk_ds_DataStorage_getColumnByName(JNIEnv *env, jobject self, jstring name);
	JNIEXPORT jobject JNICALL Java_org_temk_ds_DataStorage_addColumn(JNIEnv *env, jobject self, jobject jtype, jobject jext_type, jstring name, jobject jendian, jlong index);
	JNIEXPORT void JNICALL Java_org_temk_ds_DataStorage_flush(JNIEnv *env, jobject self);
	
    JNIEXPORT void JNICALL Java_org_temk_ds_Column_remove(JNIEnv *env, jobject self); 
    JNIEXPORT void JNICALL Java_org_temk_ds_Column_flush(JNIEnv *env, jobject self); 
	
    JNIEXPORT void JNICALL Java_org_temk_ds_Column_truncate(JNIEnv *env, jobject self, jlong new_length);    
	JNIEXPORT void JNICALL Java_org_temk_ds_Column_append(JNIEnv *env, jobject self, jobject array);	
    JNIEXPORT void JNICALL Java_org_temk_ds_Column_read(JNIEnv *env, jobject self, jlong offset, jobject indexes, jobject data);    


    JNIEXPORT jlong JNICALL Java_org_temk_ds_Column_getIndex(JNIEnv *env, jobject self);
    JNIEXPORT jlong JNICALL Java_org_temk_ds_Column_getLength(JNIEnv *env, jobject self);

    JNIEXPORT jobject JNICALL Java_org_temk_ds_Column_getExtType(JNIEnv *env, jobject self);
    JNIEXPORT jobject JNICALL Java_org_temk_ds_Column_getByteOrder(JNIEnv *env, jobject self);    
    JNIEXPORT jobject JNICALL Java_org_temk_ds_Column_getType(JNIEnv *env, jobject self);    
	
}

#endif // __DS_JNI_H__
