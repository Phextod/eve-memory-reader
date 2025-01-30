#include <stdio.h>
//#include <eve-memory-reader.h>
#include "../eve-memory-reader/eve-memory-reader.h"


int main()
{
	initialize();
	read_ui_trees();

	char* uiJson = get_ui_json();
	FILE* fp = fopen("asd.json", "wb");
	if (fp != NULL)
	{
		fputs(uiJson, fp);
		fclose(fp);
	}
}
