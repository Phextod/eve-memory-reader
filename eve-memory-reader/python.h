#pragma once

#include <stdlib.h>
#include <windows.h>
#include <winnt.h>
#include "types.h"

typedef struct {
	INT aPercent;
	INT rPercent;
	INT gPercent;
	INT bPercent;
} PyColor;

typedef struct {
	ULONGLONG address;
	char* python_object_type_name;
	BOOL is_string;
	BOOL is_unicode;
	BOOL is_int;
	BOOL is_bool;
	BOOL is_float;
	BOOL is_pycolor;
	char* string_value;
	char* unicode_value;
	INT int_value;
	BOOL bool_value;
	//float in python is 64bit, so it should be double in c
	DOUBLE float_value;
	PyColor color_value;
} PythonDictValueRepresentation;

typedef struct {
	ULONGLONG hash;
	ULONGLONG key;
	char* key_resolved;
	ULONGLONG value;
} PyDictEntry;

typedef struct {
	PyDictEntry** data;
	SIZE_T used;
	SIZE_T size;
} PyDictEntryList;

PyDictEntryList* NewPyDictEntryList();
PyDictEntry* NewPyDictEntry(ULONGLONG, ULONGLONG, ULONGLONG);
void InitPyDictEntryList(PyDictEntryList*);
void InsertPyDictEntry(PyDictEntryList*, PyDictEntry*);
void FreePythonDictValueRepresentation(PythonDictValueRepresentation*);
void FreePyDictEntry(PyDictEntry*);
void FreePyDictEntryList(PyDictEntryList*);
void PrintPyDictEntryList(PyDictEntryList*);
void PrintPyDictEntry(PyDictEntry*);


