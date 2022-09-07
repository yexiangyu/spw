#include "spw.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>

void swap_rb(Frame *frame)
{
	for (int i = 0; i < frame->w * frame->h; i++)
	{
		char tmp = frame->data[i * frame->c];
		frame->data[i * frame->c] = frame->data[i * frame->c + 2];
		frame->data[i * frame->c + 2] = tmp;
	}
}

void callback(Frame *frame)
{
	char file_name[1024] = {0};
	swap_rb(frame);
	sprintf(file_name, "%d.bmp", frame->frame_id);
	int rc = stbi_write_bmp(file_name, frame->w, frame->h, frame->c, (const void *)frame->data);
	printf("callback id=%d, frame data=%lld, output=%s, rc=%d\n", frame->frame_id, (unsigned long long)frame->data, file_name, rc);
	return;
}

int main(int argc, char **argv)
{
	Location loc = location_new("Xtium-CL_MX4_1", 2);
	printf("new location %lld\n", (unsigned long long)loc);
	// char *cfg_file = "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_cct_Default_Default.ccf";
	const char *cfg_file = "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_FullRGB_Default_Default.ccf";
	printf("cfg_file=%s\n", cfg_file);
	Acq acq = acq_new(loc, cfg_file);
	// Acq acq = acq_new(loc, "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_FullRGB_Default_Default.ccf");
	printf("new acq %lld\n", (unsigned long long)acq);
	if (acq_create(acq))
		printf("create acq\n");
	Buffer buf = buffer_new(2, acq);
	printf("new buf %lld\n", (unsigned long long)buf);
	if (buffer_create(buf))
		printf("create buf\n");
	Context ctx = context_new();
	printf("new ctx %lld\n", (unsigned long long)ctx);
	Processing proc = processing_new(buf, callback, ctx);
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
	for (int i = 0; i < 10; i++)
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