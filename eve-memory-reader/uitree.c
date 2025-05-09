#include "uitree.h"
#include "sb.h"

UITreeNodeDictEntryList* NewUITreeNodeDictEntryList()
{
	UITreeNodeDictEntryList* del = malloc(sizeof(UITreeNodeDictEntryList));
	InitUITreeNodeDictEntryList(del);
	return del;
}

UITreeNodeDictEntry* NewUITreeNodeDictEntry(char* key, PythonDictValueRepresentation* value)
{
	UITreeNodeDictEntry* e = malloc(sizeof(UITreeNodeDictEntry));
	if (e == NULL)
	{
		printf("error while allocating memory for UITreeNodeDictEntry\n");
		exit(1);
	}
	e->key = key;
	e->value = value;
	return e;
}

void InitUITreeNodeDictEntryList(UITreeNodeDictEntryList* del)
{
	SIZE_T defaultSize = 256;
	del->data = malloc(sizeof(UITreeNodeDictEntry*) * defaultSize);
	del->used = 0;
	del->size = defaultSize;
}

void InsertUITreeNodeDictEntry(UITreeNodeDictEntryList* del, UITreeNodeDictEntry* e)
{
	if (del->used == del->size)
	{
		del->size *= 2;
		del->data = realloc(del->data, del->size * sizeof(UITreeNodeDictEntry*));
		if (del->data == NULL)
		{
			printf("unable to realloc data array for UITreeNodeDictEntryList\n");
			exit(1);
		}
	}
	if (del->data == NULL)
	{
		printf("data array for UITreeNodeDictEntryList has become corrupted\n");
		exit(1);
	}
	del->data[del->used++] = e;
}

void FreeUITreeNodeDictEntry(UITreeNodeDictEntry* e)
{
	if (e == NULL)
		return;

	if (e->key != NULL)
	{
		free(e->key);
		e->key = NULL;
	}

	FreePythonDictValueRepresentation(e->value);
	free(e);
	e = NULL;
}

void FreeUITreeNodeDictEntryList(UITreeNodeDictEntryList* del)
{
	if (del == NULL)
		return;

	if (del->data != NULL)
	{
		UINT i;
		for (i = 0; i < del->used; ++i)
		{
			FreeUITreeNodeDictEntry(del->data[i]);
		}
		free(del->data);
		del->data = NULL;
	}

	free(del);
	del = NULL;
}

void FreeUITreeNode(UITreeNode* n)
{
	if (n == NULL)
		return;

	if (n->python_object_type_name != NULL)
	{
		free(n->python_object_type_name);
		n->python_object_type_name = NULL;
	}

	if (n->dict_entries_of_interest != NULL)
	{
		FreeUITreeNodeDictEntryList(n->dict_entries_of_interest);
		n->dict_entries_of_interest = NULL;
	}

	if (n->children != NULL)
	{
		UINT i;
		for (i = 0; i < n->number_of_children; ++i)
			FreeUITreeNode(n->children[i]);
		free(n->children);
		n->children = NULL;
	}

	free(n);
	n = NULL;
}

char* PrintPyColor(PyColor* color)
{
	size_t size = 100;
	char* response = malloc(sizeof(char) * size);
	if (response == NULL)
		return;

	strcpy_s(response, size, "");

	sprintf_s(response, size, "{");
	sprintf_s(response, size, "%s\"aPercent\": %d,", response, color->aPercent);
	sprintf_s(response, size, "%s\"rPercent\": %d,", response, color->rPercent);
	sprintf_s(response, size, "%s\"gPercent\": %d,", response, color->gPercent);
	sprintf_s(response, size, "%s\"bPercent\": %d", response, color->bPercent);
	sprintf_s(response, size, "%s}", response);

	return response;
}

int needs_json_escape(const char* s) {
	if (s == NULL)
		return 0;

	while(*s)
	{
		char c = *s++;
		if (c == '\"' || c == '\\' || c < 0x20) {
			return 1;
		}
	}
	return 0;
}

void escape_json_string(char** in_str) {
	if (!in_str || !*in_str) return;

	const char* input = *in_str;
	size_t len = strlen(input);

	size_t alloc_size = len;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = (unsigned char)input[i];
		if (c == '\"' || c == '\\' || c == '\n' || c == '\r' || c == '\t' || c == '\b' || c == '\f')
			alloc_size += 1;
	}

	char* output = malloc(sizeof(char) * (alloc_size + 1));
	if (!output) return;

	size_t out_i = 0;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = (unsigned char)input[i];
		switch (c)
		{
			case '\"': output[out_i++] = '\\'; output[out_i++] = '\"'; break;
			case '\\': output[out_i++] = '\\'; output[out_i++] = '\\'; break;
			case '\n': output[out_i++] = '\\'; output[out_i++] = 'n'; break;
			case '\r': output[out_i++] = '\\'; output[out_i++] = 'r'; break;
			case '\t': output[out_i++] = '\\'; output[out_i++] = 't'; break;
			case '\b': output[out_i++] = '\\'; output[out_i++] = 'b'; break;
			case '\f': output[out_i++] = '\\'; output[out_i++] = 'f'; break;
			default:
				output[out_i++] = c;
		}
	}

	output[out_i] = '\0';
	free(*in_str);
	*in_str = output;
}

char* PrintUITreeNodeDictEntryList(UITreeNodeDictEntryList* del)
{
	char* response = malloc(sizeof(char) * 5000);
	strcpy_s(response, 5000, "");
	if (del == NULL)
		return response;
	UINT i;
	for (i = 0; i < del->used; ++i)
	{
		if (del->data[i] == NULL || del->data[i]->key == NULL || del->data[i]->value == NULL)
			continue;

		if (del->data[i]->value->is_string && del->data[i]->value->string_value)
		{
			if (needs_json_escape(del->data[i]->value->string_value))
				escape_json_string(&(del->data[i]->value->string_value));
			sprintf_s(response, 5000, "%s\"%s\": ", response, del->data[i]->key);
			sprintf_s(response, 5000, "%s\"%s\",", response, del->data[i]->value->string_value);
		}
		else if (del->data[i]->value->is_unicode && del->data[i]->value->unicode_value)
		{
			if (needs_json_escape(del->data[i]->value->unicode_value))
				escape_json_string(&(del->data[i]->value->unicode_value));
			sprintf_s(response, 5000, "%s\"%s\": ", response, del->data[i]->key);
			sprintf_s(response, 5000, "%s\"%s\",", response, del->data[i]->value->unicode_value);
		}
		else if (del->data[i]->value->is_int) 
		{
			sprintf_s(response, 5000, "%s\"%s\": ", response, del->data[i]->key);
			sprintf_s(response, 5000, "%s%d,", response, del->data[i]->value->int_value);
		}
		else if (del->data[i]->value->is_float)
		{
			sprintf_s(response, 5000, "%s\"%s\": ", response, del->data[i]->key);
			sprintf_s(response, 5000, "%s%f,", response, del->data[i]->value->float_value);
		}
		else if (del->data[i]->value->is_bool)
		{
			sprintf_s(response, 5000, "%s\"%s\": ", response, del->data[i]->key);
			sprintf_s(response, 5000, "%s%s,", response, del->data[i]->value->bool_value ? "true" : "false");
		}
		else if (del->data[i]->value->is_pycolor)
		{
			sprintf_s(response, 5000, "%s\"%s\": ", response, del->data[i]->key);
			char* color_string = PrintPyColor(&del->data[i]->value->color_value);
			sprintf_s(response, 5000, "%s%s,", response, color_string);
			free(color_string);
		}
	}
	if (strlen(response) == 0)
		return response;
	if (response[strlen(response) - 1] == ',')
		response[strlen(response) - 1] = '\0';
	else
		response[strlen(response)] = '\0';
	return response;
}

char* PrintUITreeNode(UITreeNode* n, int level)
{
	StringBuilder* response = sb_create();
	if (n == NULL)
		return response;
	sb_append(response, "{");
	sb_appendf(response, "\"address\": %I64u,", n->address);
	sb_appendf(response, "\"type\": \"%s\",", n->python_object_type_name);
	sb_append(response, "\"attrs\": {");
	if (n->dict_entries_of_interest->used > 0)
	{
		char* dict_response = PrintUITreeNodeDictEntryList(n->dict_entries_of_interest);
		sb_appendf(response, "%s", dict_response);
		free(dict_response);
	}
	sb_append(response, "},");
	UINT i;
	sb_append(response, "\"children\": [");
	for (i = 0; i < n->number_of_children; ++i)
	{
		if (n->children[i] != NULL)
		{
			char* child_response = PrintUITreeNode(n->children[i], level + 1);
			sb_append(response, child_response);
			free(child_response);
			if (i < n->number_of_children - 1)
				sb_append(response, ",");
		}
	}
	sb_append(response, "]}");
	if (level == 0)
		sb_append(response, "\n");

	char* response_str = sb_concat(response);
	sb_free(response);

	return response_str;
}

