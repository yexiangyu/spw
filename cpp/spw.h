#pragma once

#ifdef _WIN64
#define DLL __declspec(dllexport)
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#else
#define bool int
#endif

	DLL typedef struct Frame
	{
		uint64_t w;
		uint64_t h;
		uint64_t c;
		uint64_t timestamp;
		uint64_t frame_id;
		unsigned char *data;
	} Frame;

	DLL typedef void (*FrameCallback)(Frame *);

	DLL typedef void *Location;
	DLL Location location_new(const char *server_name, int device_id);
	DLL bool location_free(Location location);

	DLL typedef void *Acq;
	DLL Acq acq_new(Location location, const char *config_file_name);
	DLL bool acq_free(Acq acq);
	DLL bool acq_create(Acq acq);

	DLL typedef void *Buffer;
	DLL Buffer buffer_new(int count, Acq acq);
	DLL bool buffer_free(Buffer buf);
	DLL bool buffer_create(Buffer buf);

	DLL typedef void *Context;
	DLL Context context_new();
	DLL bool context_free(Context ctx);
	DLL int context_grab_frame_count(Context ctx);
	DLL int context_proc_frame_count(Context ctx);
	DLL void context_counter_reset(Context ctx);

	DLL typedef void *Processing;
	DLL Processing processing_new(Buffer buf, FrameCallback callback, Context ctx);
	DLL bool processing_free(Processing proc);
	DLL bool processing_create(Processing proc);

	DLL void context_attach_processing(Context ctx, Processing proc);

	DLL typedef void *AcqToBuf;
	DLL AcqToBuf acq_to_buffer_new(Acq acq, Buffer buf, Context ctx);
	DLL bool acq_to_buffer_free(AcqToBuf atb);
	DLL bool acq_to_buffer_create(AcqToBuf atb);
	DLL bool acq_to_buffer_grab(AcqToBuf atb);
	DLL bool acq_to_buffer_freeze(AcqToBuf atb);

	DLL void sleep_for_1s();

#ifdef __cplusplus
}
#endif
#endif
