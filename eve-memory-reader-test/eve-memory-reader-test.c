#include <stdio.h>
//#include <eve-memory-reader.h>
#include "../eve-memory-reader/eve-memory-reader.h"


int main()
{
	initialize();
	read_ui_trees();

	char* uiJson = get_ui_json();
	FILE* fp = fopen("asd.json", "ab");
	if (fp != NULL)
	{
		fputs(uiJson, fp);
		fclose(fp);
	}

	read_ui_trees_from_address(2267931950496);
	char* uiJson2 = get_ui_json();
	FILE* fp2 = fopen("asd2.json", "ab");
	if (fp2 != NULL)
	{
		fputs(uiJson2, fp2);
		fclose(fp2);
	}

}
