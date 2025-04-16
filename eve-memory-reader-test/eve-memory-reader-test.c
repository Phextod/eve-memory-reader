#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>
//#include <eve-memory-reader.h>
#include "../eve-memory-reader/eve-memory-reader.h"


int main()
{
	initialize(0);
	read_ui_trees();

	char* uiJson = get_ui_json();
	FILE* fp = fopen("asd.json", "wb");
	if (fp != NULL)
	{
		fputs(uiJson, fp);
		fclose(fp);
	}

	cleanup();
	_CrtDumpMemoryLeaks();
}
