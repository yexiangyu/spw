#include "spw.h"

int main(int argc, char **argv)
{
	char *svr_name = "fuck";
	unsigned int dev_num = 1;
	sap_location_new(svr_name, dev_num);
	return 0;
}