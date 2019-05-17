#include <stdio.h>
#include <stdlib.h>
#include "menu.h"


/**************************************** static funcs *******************************************/


int main()
{

	frameMgrCreate();
	//checkMenuConfig();
	parseMenuConfig();
	
	//showTotal();
	menuFrameworkRun();

	system("pause");
	frameMgrDestroy();
	system("pause");

}


