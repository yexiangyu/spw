#pragma once

#ifdef _WIN64
#define EXPORT __declspec(dllexport)
#else
// #define EXPORT __attribute__((visibility("visible")))
#define EXPORT
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	EXPORT typedef void *SapAcquisitionW;
	EXPORT typedef void *SapAcqDeviceW;
	EXPORT typedef void *SapBufferWithTrashW;
	EXPORT typedef void *SapTransferW;
	EXPORT typedef void *SapAcqToBufW;
	EXPORT typedef void *SapViewW;
	EXPORT typedef void *SapLocationW;
	EXPORT typedef void *SapCallbackW;

	EXPORT SapLocationW sap_location_new(char *acq_server_name, unsigned int acq_device_number);
	EXPORT int sap_manager_get_resource_count(char *acq_server_name);
	EXPORT SapAcquisitionW sap_acquisition_new(SapLocationW loc, char *config_file_name);
	EXPORT int sap_acquisition_create(SapAcquisitionW acq);
	EXPORT SapBufferWithTrashW sap_buffer_with_trash_new(unsigned int count, SapAcquisitionW acq);
	EXPORT int sap_buffer_with_trash_create(SapBufferWithTrashW buf);
	EXPORT SapViewW sap_view_new(SapBufferWithTrashW buf);
	EXPORT int sap_view_destroy(SapViewW view);
	EXPORT SapAcqToBufW sap_acq_to_buf_new(SapAcquisitionW acq, SapBufferWithTrashW buf, SapCallbackW callback, SapViewW view);
	EXPORT int sap_acq_to_buf_destroy(SapAcqToBufW sat);
	EXPORT int sap_acq_to_buf_create(SapAcqToBufW sat);
	EXPORT int sap_acq_to_buf_grab(SapAcqToBufW sat);
	EXPORT int sap_acq_to_buf_freeze(SapAcqToBufW sat);
	EXPORT int sap_acq_to_buf_unresigter_callback(SapAcqToBufW sat);
	EXPORT int sap_acq_to_buf_wait(SapAcqToBufW sat, unsigned int duration);
#ifdef __cplusplus
}
#endif