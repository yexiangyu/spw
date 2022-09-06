#pragma once

#ifdef _WIN64
#define DLL __declspec(dllexport)
#ifdef __cplusplus
extern "C"
{
#else
#define bool int
#endif

	DLL typedef struct Frame
	{
		int w;
		int h;
		int c;
		char *data;
	} Frame;

	DLL typedef void (*FrameCallback)(Frame);

	DLL typedef void *Location;
	DLL Location location_new(char *server_name, int device_id);
	DLL bool location_free(Location location);

	DLL typedef void *Acq;
	DLL Acq acq_new(Location location, char *config_file_name);
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

	DLL typedef void *Processing;
	DLL Processing processing_new(Buffer buf, FrameCallback callback, Context ctx);
	DLL bool procssing_free(Processing proc);
	DLL bool processing_create(Processing proc);

	DLL void context_attach_processing(Context ctx, Processing proc);

	DLL typedef void *AcqToBuf;
	DLL AcqToBuf acq_to_buffer_new(Acq acq, Buffer buf, Context ctx);
	DLL bool acq_to_buffer_free(AcqToBuf atb);
	DLL bool acq_to_buffer_create(AcqToBuf atb);
	DLL bool acq_to_buffer_grab(AcqToBuf atb);
	DLL bool acq_to_buffer_freeze(AcqToBuf atb);

	DLL void sleep_for_1s();

	// DLL typedef struct Frame
	// {
	// 	int w;
	// 	int h;
	// 	int c;
	// 	char *data;
	// } Frame;

	// typedef void (*FrameCallback)(Frame);

	// DLL typedef struct Device
	// {
	// 	char name[64];
	// } Device;

	// DLL typedef struct Server
	// {
	// 	char name[64];
	// 	Device acq_devices[16];
	// 	Device acqs[16];
	// 	int acq_device_num;
	// 	int acq_num;
	// } Server;

	// DLL int get_server_count();
	// DLL int get_server_by_index(int idx, Server *svr);
	// DLL void *new_location(char *server_name, int device_id);
	// DLL void free_location(void *loc);
	// DLL void *new_device(void *loc, char *config_file_name);
	// DLL bool create_device(void *device);
	// DLL bool destroy_device(void *device);
	// DLL void free_device(void *device);
	// DLL bool create_device(void *device);
	// DLL bool destroy_device(void *device);
	// DLL void *new_acq(void *loc, char *config_file_name);
	// DLL bool create_acq(void *acq);
	// DLL bool destroy_acq(void *acq);
	// DLL void free_acq(void *acq);
	// DLL bool create_acq(void *acq);
	// DLL bool destroy_acq(void *acq);
	// DLL void free_acq(void *acq);
	// DLL void *new_buffer_with_trash(int count, void *device);
	// DLL void free_buffer_with_trash(void *buf);
	// DLL bool create_buffer_with_trash(void *buf);
	// DLL bool destroy_buffer_with_trash(void *buf);
	// DLL void *new_transfer_context();
	// DLL void free_transfer_context(void *ctx);
	// DLL void attach_processing_to_context(void *ctx, void *proc);
	// DLL int get_grab_frame_count_from_context(void *ctx);
	// DLL int get_proc_frame_count_from_context(void *ctx);
	// DLL void *new_acq_device_to_buffer(void *device, void *buf, void *ctx);
	// DLL bool destroy_acq_device_to_buffer(void *trans);
	// DLL bool create_acq_device_to_buffer(void *trans);
	// DLL void set_acq_device_to_buffer_auto_empty(void *trans, bool val);
	// DLL bool grab_acq_device_to_buffer(void *trans);
	// DLL bool freeze_acq_device_to_buffer(void *trans);
	// DLL bool wait_acq_device_to_buffer(void *trans, int duration);
	// DLL void free_acq_device_to_buffer(void *trans);
	// DLL void *new_bw_processing(void *buf, FrameCallback f_callback, void *ctx);
	// DLL void free_bw_processing(void *proc);
	// DLL bool create_bw_processing(void *proc);
	// DLL void set_bw_process_auto_empty(void *proc, bool val);
	// DLL void init_bw_process(void *proc);

#ifdef __cplusplus
}
#endif
#endif

// #pragma once

// #ifdef _WIN64
// #include <stdint.h>
// #define EXPORT __declspec(dllexport)
// #else
// // #define EXPORT __attribute__((visibility("visible")))
// #define EXPORT
// #endif

// #ifdef __cplusplus
// extern "C"
// {
// #else
// #define bool int
// #endif

// 	typedef struct RGBFrame
// 	{
// 		int h;
// 		int w;
// 		uint8_t *data;

// 	} RGBFrame;

// 	typedef void (*RGBFrameCallback)(RGBFrame);

// 	EXPORT typedef void *SapAcquisitionW;
// 	EXPORT typedef void *SapAcqDeviceW;
// 	EXPORT typedef void *SapBufferWithTrashW;
// 	EXPORT typedef void *SapTransferW;
// 	EXPORT typedef void *SapAcqToBufW;
// 	EXPORT typedef void *SapViewW;
// 	EXPORT typedef void *SapLocationW;
// 	EXPORT typedef void *SapCallbackW;

// 	EXPORT SapLocationW sap_location_new(char *acq_server_name, unsigned int acq_device_number);
// 	EXPORT int sap_manager_get_server_count();
// 	EXPORT int sap_manager_get_server_index(char *acq_server_name);
// 	EXPORT int sap_manager_get_resource_device_count(char *acq_server_name);
// 	EXPORT int sap_manager_get_resource_camera_count(char *acq_server_name);
// 	EXPORT bool sap_manager_get_server_name_by_index(int i, char *server_name);
// 	EXPORT SapAcquisitionW sap_acquisition_new(SapLocationW loc, char *config_file_name);
// 	EXPORT bool sap_acquisition_create(SapAcquisitionW acq);
// 	EXPORT SapBufferWithTrashW sap_buffer_with_trash_new(unsigned int count, SapAcquisitionW acq);
// 	EXPORT bool sap_buffer_with_trash_create(SapBufferWithTrashW buf);
// 	EXPORT SapViewW sap_view_new(SapBufferWithTrashW buf);
// 	EXPORT bool sap_view_create(SapViewW view);
// 	EXPORT bool sap_view_destroy(SapViewW view);
// 	EXPORT SapAcqToBufW sap_acq_to_buf_new(SapAcquisitionW acq, SapBufferWithTrashW buf, SapCallbackW callback, SapViewW view);
// 	EXPORT bool sap_acq_to_buf_destroy(SapAcqToBufW sat);
// 	EXPORT bool sap_acq_to_buf_create(SapAcqToBufW sat);
// 	EXPORT bool sap_acq_to_buf_grab(SapAcqToBufW sat);
// 	EXPORT bool sap_acq_to_buf_freeze(SapAcqToBufW sat);
// 	EXPORT bool sap_acq_to_buf_unresigter_callback(SapAcqToBufW sat);
// 	EXPORT bool sap_acq_to_buf_wait(SapAcqToBufW sat, int duration);
// 	EXPORT void dump_callback(void *info);
// #ifdef __cplusplus
// }
// #endif