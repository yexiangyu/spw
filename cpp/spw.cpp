#include <stdlib.h>
#include "spw.h"
#ifdef _WIN32
#include "sapclassbasic.h"
#else
#endif

extern "C"
{
	SapLocationW sap_location_new(char *acq_server_name, unsigned int acq_device_number)
	{
#ifdef _WIN32
		return new SapLocation(acq_server_name, acq_device_number);
#else
		return NULL;
#endif
	}

	int sap_manager_get_resource_count(char *acq_server_name)
	{
#ifdef _WIN32
		return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
#else
		return -1;
#endif
	}

	SapAcquisitionW sap_acquisition_new(SapLocationW loc, char *config_file_name)
	{
#ifdef _WIN32
		// return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
		// SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq)
		return SapAcquisition(*loc, configFilename)
#else
		return NULL;
#endif
	}
	int sap_acquisition_create(SapAcquisitionW acq)
	{
#ifdef _WIN32
		// return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
		// SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq)
		// return SapAcquisition(*loc, configFilename)
		return -1;
#else
		return -1;
#endif
	}
	SapBufferWithTrashW sap_buffer_with_trash_new(unsigned int count, SapAcquisitionW acq)
	{
#ifdef _WIN32
		// return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
		// SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq)
		// return SapAcquisition(*loc, configFilename)
		return NULL;
#else
		return NULL;
#endif
	}

	int sap_buffer_with_trash_create(SapBufferWithTrashW buf)
	{
#ifdef _WIN32
		// return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
		// SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq)
		// return SapAcquisition(*loc, configFilename)
		return -1;
#else
		return -1;
#endif
	}

	SapViewW sap_view_new(SapBufferWithTrashW buf)
	{
#ifdef _WIN32
		// return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
		// SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq)
		// return SapAcquisition(*loc, configFilename)
		return NULL;
#else
		return NULL;
#endif
	}
	int sap_view_destroy(SapViewW view)
	{
		return -1;
	}
	SapAcqToBufW sap_acq_to_buf_new(SapAcquisitionW acq, SapBufferWithTrashW buf, SapCallbackW callback, SapViewW view)
	{
		return NULL;
	}
	int sap_acq_to_buf_destroy(SapAcqToBufW sat)
	{
		return -1;
	}
	int sap_acq_to_buf_create(SapAcqToBufW sat)
	{
		return -1;
	}
	int sap_acq_to_buf_grab(SapAcqToBufW sat)
	{
		return -1;
	}
	int sap_acq_to_buf_freeze(SapAcqToBufW sat)
	{
		return -1;
	}
	int sap_acq_to_buf_unresigter_callback(SapAcqToBufW sat)
	{
		return -1;
	}
	int sap_acq_to_buf_wait(SapAcqToBufW sat, unsigned int duration)
	{
		return -1;
	}
}