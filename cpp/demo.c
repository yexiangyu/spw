#include "spw.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	Location loc = location_new("Xtium-CL_MX4_1", 1);
	printf("new location %lld\n", (unsigned long long)loc);
	Acq acq = acq_new(loc, "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_cct_Default_Default.ccf");
	printf("new acq %lld\n", (unsigned long long)acq);
	if (acq_create(acq))
		printf("create acq\n");
	Buffer buf = buffer_new(2, acq);
	printf("new buf %lld\n", (unsigned long long)buf);
	if (buffer_create(buf))
		printf("create buf\n");
	Context ctx = context_new();
	printf("new ctx %lld\n", (unsigned long long)ctx);
	Processing proc = processing_new(buf, 0, ctx);
	printf("new proc %lld\n", (unsigned long long)proc);
	if (processing_create(proc))
		printf("create proc\n");
	context_attach_processing(ctx, proc);
	AcqToBuf atb = acq_to_buffer_new(acq, buf, ctx);
	printf("new atb %lld\n", (unsigned long long)atb);
	if (acq_to_buffer_create(atb))
		printf("create atb\n");
	if (acq_to_buffer_grab(atb))
		printf("grab atb\n");
	for (int i = 0; i < 10000; i++)
	{
		sleep_for_1s();
	}
	// {
	// printf("current grab frame count %d\n", i);
	// sleep_for_1s();
	// }
	acq_to_buffer_freeze(atb);
	return 0;
}