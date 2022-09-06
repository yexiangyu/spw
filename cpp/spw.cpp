#include "spw.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

// #include "spw.hpp"

#ifdef _WIN64
#include "SapClassBasic.h"

template <class T>
bool free_as(void *ptr)
{
	if (ptr)
	{
		delete (T *)ptr;
		return true;
	}
	return false;
}

template <class T>
bool free_destroy_as(void *ptr)
{
	if (ptr)
	{
		auto ptr_ = (T *)ptr;
		auto rc = ptr_->Destroy();
		delete ptr_;
		return rc;
	}
	return false;
}

template <class T>
bool create_as(void *ptr)
{
	if (ptr)
	{
		auto ptr_ = (T *)ptr;
		return ptr_->Create();
	}
	return false;
}

class Processing_ : public SapProcessing
{
public:
	Processing_(SapBuffer *buf, FrameCallback f_callback_, SapProCallback p_callback, void *ctx);
	virtual ~Processing_();

protected:
	virtual BOOL Run();
	FrameCallback f_callback;
};

Processing_::Processing_(SapBuffer *buf, FrameCallback f_callback_, SapProCallback p_callback, void *ctx)
	: SapProcessing(buf, p_callback, ctx)
{
	f_callback = f_callback_;
}

Processing_::~Processing_()
{
	if (m_bInitOK)
		Destroy();
}

struct Context_;

BOOL Processing_::Run()
{
	int buf_idx = GetIndex();
	auto ctx = (Context_ *)GetContext();
	SapBuffer::State state = 0;
	if (!m_pBuffers->GetState(buf_idx, &state))
	{
		std::cerr << "failed to get state" << std::endl;
		return false;
	}

	if (state == SapBuffer::StateFull)
	{
		void *addr = nullptr;
		int mem_size = 0;
		m_pBuffers->GetAddress(buf_idx, &addr);
		m_pBuffers->GetSpaceUsed(buf_idx, &mem_size);
		int w = m_pBuffers->GetWidth();
		int h = m_pBuffers->GetHeight();
		std::cout << "current buff index=" << buf_idx << ",state=" << state << ",addr=" << addr << ",mem_size=" << mem_size << ",size=" << w << "x" << h << std::endl;
	}
	else
	{
		std::cout << "current buff index=" << buf_idx << ",state=" << state << std::endl;
	}

	return true;
}

struct Context_
{
	std::atomic_int frame_grab_count = 0;
	std::atomic_int frame_proc_count = 0;
	Processing_ *processing;
};

void transfer_callback(SapXferCallbackInfo *info)
{
	auto context = (Context_ *)info->GetContext();
	context->frame_grab_count++;
	if (!info->IsTrash())
	{
		context->processing->ExecuteNext();
	}
}

void processing_callback(SapProCallbackInfo *info)
{
	auto context = (Context_ *)info->GetContext();
	context->frame_proc_count++;
}

extern "C"
{
	DLL Location location_new(char *server_name, int device_id)
	{
		return new SapLocation(server_name, device_id);
	}

	DLL bool location_free(Location location)
	{
		return free_as<SapLocation>(location);
	}

	DLL Acq acq_new(Location location, char *config_file_name)
	{
		return new SapAcquisition(*(SapLocation *)location, config_file_name);
	}

	DLL bool acq_create(Acq acq)
	{
		return create_as<SapAcquisition>(acq);
	}

	DLL Buffer buffer_new(int count, Acq acq)
	{
		return new SapBufferWithTrash(count, (SapAcquisition *)acq);
	}

	DLL bool buffer_free(Buffer buf)
	{
		return free_destroy_as<SapBufferWithTrash>(buf);
	}

	DLL bool buffer_create(Buffer buf)
	{
		return create_as<SapBufferWithTrash>(buf);
	}

	DLL Context context_new()
	{
		return new Context_{};
	}

	DLL bool context_free(Context ctx)
	{
		return free_as<Context_>(ctx);
	}

	DLL Processing processing_new(Buffer buf, FrameCallback callback, Context ctx)
	{
		return new Processing_((SapBuffer *)buf, callback, processing_callback, ctx);
	}

	DLL int context_grab_frame_count(Context ctx)
	{
		return ((Context_ *)ctx)->frame_grab_count;
	}
	DLL int context_proc_frame_count(Context ctx)
	{
		return ((Context_ *)ctx)->frame_proc_count;
	}

	DLL bool procssing_free(Processing proc)
	{
		return free_destroy_as<Processing_>(proc);
	}

	DLL bool processing_create(Processing proc)
	{
		return create_as<Processing_>(proc);
	}

	DLL void context_attach_processing(Context ctx, Processing proc)
	{
		((Context_ *)ctx)->processing = (Processing_ *)proc;
	}

	DLL AcqToBuf acq_to_buffer_new(Acq acq, Buffer buf, Context ctx)
	{
		return new SapAcqToBuf((SapAcquisition *)acq, (SapBuffer *)buf, transfer_callback, ctx);
	}
	DLL bool acq_to_buffer_free(AcqToBuf atb)
	{
		return free_destroy_as<SapAcqToBuf>(atb);
	}

	DLL bool acq_to_buffer_create(AcqToBuf atb)
	{
		return create_as<SapAcqToBuf>(atb);
	}
	DLL bool acq_to_buffer_grab(AcqToBuf atb)
	{
		return ((SapAcqToBuf *)atb)->Grab();
	}

	DLL bool acq_to_buffer_freeze(AcqToBuf atb)
	{
		return ((SapAcqToBuf *)atb)->Freeze();
	}

	void sleep_for_1s()
	{
		std::this_thread::sleep_for(1s);
	}
}

#endif

// #include <stdexcept>
// #include <iostream>
// #include <atomic>

// #define CHECK_BOOL(x, y)             \
// 	if (!x)                          \
// 	{                                \
// 		std::cerr << y << std::endl; \
// 		return 0;                    \
// 	}

// #define FREE(t, x) free((t *)x)
// #define AS(t, x) ((t *)x)

// class BWProcessing : public SapProcessing
// {
// public:
// 	BWProcessing(SapBuffer *pBuffers, FrameCallback fCallback, SapProCallback pCallback, void *pContext);
// 	virtual ~BWProcessing();

// protected:
// 	virtual BOOL Run();
// 	FrameCallback m_fCallback;
// };

// BWProcessing::BWProcessing(SapBuffer *pBuffers, FrameCallback fCallback, SapProCallback pCallback, void *pContext)
// 	: SapProcessing(pBuffers, pCallback, pContext)
// {
// 	m_fCallback = fCallback;
// }

// BWProcessing::~BWProcessing()
// {
// 	if (m_bInitOK)
// 		Destroy();
// }

// BOOL BWProcessing::Run()
// {
// 	auto buf_idx = GetIndex();
// 	SapBuffer::State state;
// 	auto buf_ok = m_pBuffers->GetState(buf_idx, &state) && state == SapBuffer::StateFull;

// 	if (buf_ok)
// 	{
// 		void *in_addr = nullptr;
// 		int in_size = 0;
// 		m_pBuffers->GetAddress(buf_idx, &in_addr);
// 		m_pBuffers->GetSpaceUsed(buf_idx, &in_size);
// 		auto w = m_pBuffers->GetWidth();
// 		auto h = m_pBuffers->GetHeight();
// 		auto c = in_size / (w * h);
// 		// if (m_pBuffers->GetFormat() != SapFormatRGB888 || w * h * 3 != in_size)
// 		// {
// 		// 	throw std::runtime_error("not rgb");
// 		// }

// 		if (m_fCallback)
// 		{
// 			char *data = (char *)malloc(in_size);
// 			std::copy((uint8_t *)in_addr, (uint8_t *)(in_addr) + in_size, data);
// 			auto buf = Frame{
// 				w,
// 				h,
// 				c,
// 				data};
// 			m_fCallback(buf);
// 		}
// 		else
// 		{
// 			printf("capture frame: %dx%d, size=%d", w, h, in_size);
// 		}
// 	}

// 	return true;
// }

// struct TransferContext
// {
// 	std::atomic_int frameGrabCount = 0, frameProcessingCount = 0;
// 	BWProcessing *processing;
// };

// void transferCallback(SapXferCallbackInfo *info)
// {
// 	auto context = (TransferContext *)info->GetContext();

// 	context->frameGrabCount++;
// 	if (!info->IsTrash())
// 	{
// 		// Execute Run() for this frame
// 		context->processing->ExecuteNext();
// 	}
// }

// void processingCallback(SapProCallbackInfo *info)
// {
// 	auto context = (TransferContext *)info->GetContext();

// 	// Processing has finished
// 	context->frameProcessingCount++;
// }

// extern "C"
// {
// 	DLL int get_server_count()
// 	{
// 		return SapManager::GetServerCount();
// 	}

// 	DLL int get_server_by_index(int idx, Server *svr)
// 	{
// 		CHECK_BOOL(SapManager::GetServerName(idx, svr->name), "get server name failed\n");
// 		svr->acq_device_num = SapManager::GetResourceCount(idx, SapManager::ResourceAcqDevice);
// 		for (int i = 0; i < svr->acq_device_num; i++)
// 		{
// 			CHECK_BOOL(SapManager::GetResourceName(svr->name, SapManager::ResourceAcqDevice, i, svr->acq_devices[i].name), "get acq device name failed");
// 		}
// 		svr->acq_num = SapManager::GetResourceCount(idx, SapManager::ResourceAcq);
// 		for (int i = 0; i < svr->acq_num; i++)
// 		{
// 			CHECK_BOOL(SapManager::GetResourceName(svr->name, SapManager::ResourceAcq, i, svr->acqs[i].name), "get acq device name failed");
// 		}
// 		return 1;
// 	}

// 	DLL void *new_location(char *server_name, int device_id)
// 	{
// 		return new SapLocation(server_name, device_id);
// 	}

// 	DLL void free_location(void *loc)
// 	{
// 		FREE(SapLocation, loc);
// 		return;
// 	}

// 	DLL void *new_device(void *loc, char *config_file_name)
// 	{
// 		auto dev = new SapAcqDevice(*AS(SapLocation, loc), config_file_name);
// 		return dev;
// 	}

// 	DLL bool create_device(void *device)
// 	{
// 		return AS(SapAcqDevice, device)->Create();
// 	}

// 	DLL bool destroy_device(void *device)
// 	{
// 		return AS(SapAcqDevice, device)->Destroy();
// 	}

// 	DLL void free_device(void *device)
// 	{
// 		FREE(SapAcqDevice, device);
// 		return;
// 	}

// 	DLL void *new_acq(void *loc, char *config_file_name)
// 	{
// 		auto dev = new SapAcquisition(*AS(SapLocation, loc), config_file_name);
// 		return dev;
// 	}

// 	DLL bool create_acq(void *acq)
// 	{
// 		return AS(SapAcquisition, acq)->Create();
// 	}

// 	DLL bool destroy_acq(void *acq)
// 	{
// 		return AS(SapAcquisition, acq)->Destroy();
// 	}

// 	DLL void free_acq(void *acq)
// 	{
// 		FREE(SapAcquisition, acq);
// 		return;
// 	}

// 	DLL void *new_buffer_with_trash(int count, void *device)
// 	{
// 		auto buf = new SapBufferWithTrash(count, AS(SapAcqDevice, device));
// 		return buf;
// 	}

// 	DLL void free_buffer_with_trash(void *buf)
// 	{
// 		FREE(SapBufferWithTrash, buf);
// 		return;
// 	}

// 	DLL bool create_buffer_with_trash(void *buf)
// 	{
// 		return AS(SapBufferWithTrash, buf)->Create();
// 	}

// 	DLL bool destroy_buffer_with_trash(void *buf)
// 	{
// 		return AS(SapBufferWithTrash, buf)->Destroy();
// 	}

// 	DLL void *new_transfer_context()
// 	{
// 		return new TransferContext();
// 	}

// 	DLL void free_transfer_context(void *ctx)
// 	{
// 		FREE(TransferContext, ctx);
// 		return;
// 	}

// 	DLL int get_grab_frame_count_from_context(void *ctx)
// 	{
// 		return AS(TransferContext, ctx)->frameGrabCount;
// 	}

// 	DLL int get_proc_frame_count_from_context(void *ctx)
// 	{
// 		return AS(TransferContext, ctx)->frameProcessingCount;
// 	}

// 	DLL void attach_processing_to_context(void *ctx, void *proc)
// 	{
// 		AS(TransferContext, ctx)->processing = AS(BWProcessing, proc);
// 		return;
// 	}

// 	DLL void *new_acq_device_to_buffer(void *device, void *buf, void *ctx)
// 	{
// 		return new SapAcqDeviceToBuf(AS(SapAcqDevice, device), AS(SapBuffer, buf), transferCallback, ctx);
// 	}

// 	DLL bool destroy_acq_device_to_buffer(void *trans)
// 	{
// 		return AS(SapAcqDeviceToBuf, trans)->Destroy();
// 	}

// 	DLL bool create_acq_device_to_buffer(void *trans)
// 	{
// 		return AS(SapAcqDeviceToBuf, trans)->Create();
// 	}

// 	DLL void set_acq_device_to_buffer_auto_empty(void *trans, bool val)
// 	{
// 		AS(SapAcqDeviceToBuf, trans)->SetAutoEmpty(val);
// 		return;
// 	}

// 	DLL bool grab_acq_device_to_buffer(void *trans)
// 	{
// 		return AS(SapAcqDeviceToBuf, trans)->Grab();
// 	}

// 	DLL bool freeze_acq_device_to_buffer(void *trans)
// 	{
// 		return AS(SapAcqDeviceToBuf, trans)->Freeze();
// 	}

// 	DLL bool wait_acq_device_to_buffer(void *trans, int duration)
// 	{
// 		return AS(SapAcqDeviceToBuf, trans)->Wait(duration);
// 	}

// 	DLL void free_acq_device_to_buffer(void *trans)
// 	{
// 		FREE(SapAcqDeviceToBuf, trans);
// 		return;
// 	}

// 	DLL void *new_bw_processing(void *buf, FrameCallback f_callback, void *ctx)
// 	{

// 		return new BWProcessing(AS(SapBuffer, buf), f_callback, processingCallback, ctx);
// 	}

// 	DLL void free_bw_processing(void *proc)
// 	{

// 		FREE(BWProcessing, proc);
// 		return;
// 	}

// 	DLL bool create_bw_processing(void *proc)
// 	{
// 		return AS(BWProcessing, proc)->Create();
// 	}

// 	DLL void set_bw_process_auto_empty(void *proc, bool val)
// 	{
// 		AS(BWProcessing, proc)->SetAutoEmpty(val);
// 		return;
// 	}

// 	DLL void init_bw_process(void *proc)
// 	{
// 		AS(BWProcessing, proc)->Init();
// 		return;
// 	}
// }
// #endif

// // #include <stdlib.h>
// // #include <stdexcept>
// // #include "spw.h"
// // #ifdef _WIN32
// // #include "SapClassBasic.h"
// // #include "conio.h"
// // #include <vector>
// // #include <atomic>
// // #include <memory>

// // int getServerCount()
// // {
// // 	return SapManager::GetServerCount();
// // }

// // bool getServerNameByIndex(int idx, char *server_name)
// // {
// // 	return SapManager::GetServerName(idx, server_name);
// // }

// // int getDeviceCount(char *server_name)
// // {
// // 	return SapManager::GetResourceCount(server_name, SapManager::ResourceAcq);
// // }

// // int getCameraCount(char *server_name)
// // {
// // 	return SapManager::GetResourceCount(server_name, SapManager::ResourceAcqDevice);
// // }

// // SapLocation *newLocation(char *server_name, unsigned int device_index)
// // {
// // 	return new SapLocation(server_name, device_index);
// // }

// // SapAcqDevice *newDevice(SapLocation *loc, char *config_file_name)
// // {
// // 	auto device = new SapAcqDevice(*loc, config_file_name);
// // 	if (!device->Create())
// // 	{
// // 		throw std::runtime_error("Failed to create camera object.");
// // 	}
// // 	return device;
// // }

// // SapBufferWithTrash *newBufferWithTrash(int count, SapAcqDevice *device)
// // {
// // 	auto buf = new SapBufferWithTrash(count, device);
// // 	if (!buf->Create())
// // 	{
// // 		throw std::runtime_error("Failed to create buffer object.");
// // 	}
// // 	return buf;
// // }

// // class Processing : public SapProcessing
// // {
// // public:
// // 	Processing(SapBuffer *pBuffers, RGBFrameCallback fCallback, SapProCallback pCallback, void *pContext);
// // 	virtual ~Processing();

// // protected:
// // 	virtual BOOL Run();
// // 	RGBFrameCallback m_fCallback;
// // };

// // Processing::Processing(SapBuffer *pBuffers, RGBFrameCallback fCallback, SapProCallback pCallback, void *pContext)
// // 	: SapProcessing(pBuffers, pCallback, pContext)
// // {
// // 	m_fCallback = fCallback;
// // }

// // Processing::~Processing()
// // {
// // 	if (m_bInitOK)
// // 		Destroy();
// // }

// // BOOL Processing::Run()
// // {
// // 	auto buf_idx = GetIndex();
// // 	SapBuffer::State state;
// // 	auto buf_ok = m_pBuffers->GetState(buf_idx, &state) && state == SapBuffer::StateFull;

// // 	if (buf_ok)
// // 	{
// // 		void *in_addr = nullptr;
// // 		int in_size = 0;
// // 		m_pBuffers->GetAddress(buf_idx, &in_addr);
// // 		m_pBuffers->GetSpaceUsed(buf_idx, &in_size);
// // 		auto w = m_pBuffers->GetWidth();
// // 		auto h = m_pBuffers->GetHeight();
// // 		if (m_pBuffers->GetFormat() != SapFormatRGB888 || w * h * 3 != in_size)
// // 		{
// // 			throw std::runtime_error("not rgb");
// // 		}

// // 		if (m_fCallback)
// // 		{
// // 			uint8_t *data = (uint8_t *)malloc(in_size);
// // 			std::copy((uint8_t *)in_addr, (uint8_t *)(in_addr) + in_size, data);
// // 			auto buf = RGBFrame{
// // 				w,
// // 				h,
// // 				data};
// // 			m_fCallback(buf);
// // 		}
// // 		else
// // 		{
// // 			printf("capture frame: %dx%d, size=%d", w, h, in_size);
// // 		}
// // 	}

// // 	return true;
// // }

// // struct TransferContext
// // {
// // 	std::atomic_int frameGrabCount = 0, frameProcessingCount = 0;
// // 	Processing *processing;
// // };

// // void transferCallback(SapXferCallbackInfo *info)
// // {
// // 	auto context = (TransferContext *)info->GetContext();

// // 	context->frameGrabCount++;
// // 	if (!info->IsTrash())
// // 	{
// // 		// Execute Run() for this frame
// // 		context->processing->ExecuteNext();
// // 	}
// // }

// // void processingCallback(SapProCallbackInfo *info)
// // {
// // 	auto context = (TransferContext *)info->GetContext();

// // 	// Processing has finished
// // 	context->frameProcessingCount++;
// // }

// // SapAcqDeviceToBuf *newTransferDeviceToBuffer(SapAcqDevice *device, SapBuffer *buf, TransferContext *ctx)
// // {
// // 	auto trans = new SapAcqDeviceToBuf(device, buf, transferCallback, (void *)ctx);
// // 	if (!trans->Create())
// // 	{
// // 		throw std::runtime_error("Failed to create transfer object.");
// // 	}
// // 	return trans;
// // }

// // void easyGrab(char *server_name, int device_index)
// // {
// // }

// // #endif

// // extern "C"
// // {
// // 	int get_server_count()
// // 	{
// // 		return getServerCount();
// // 	}

// // 	bool get_server_name_by_index(int idx, char *server_name)
// // 	{
// // 		return getServerNameByIndex(idx, server_name);
// // 	}

// // 	int get_device_count(char *server_name)
// // 	{
// // 		return getDeviceCount(server_name);
// // 	}

// // 	int get_camera_count(char *server_name)
// // 	{
// // 		return getCameraCount(server_name);
// // 	}

// // 	void *new_location(char *server_name, unsigned int device_index)
// // 	{
// // 		return (void *)newLocation(server_name, device_index);
// // 	}

// // 	void free_location(void *loc)
// // 	{
// // 		free((SapLocation *)loc);
// // 		return;
// // 	}

// // 	void *new_device(void *loc, char *config_file_name)
// // 	{
// // 		return (void *)newDevice((SapLocation *)loc, config_file_name);
// // 	}

// // 	void free_device(void *device)
// // 	{
// // 		((SapAcqDevice *)device)->Destroy();
// // 		delete (SapAcqDevice *)device;
// // 		return;
// // 	}

// // 	void *new_buffer_with_trash(int count, void *device)
// // 	{
// // 		return newBufferWithTrash(count, (SapAcqDevice *)device);
// // 	}

// // 	void free_buffer_with_trash(void *buf)
// // 	{
// // 		((SapBufferWithTrash *)buf)->Destroy();
// // 		delete (SapBufferWithTrash *)buf;
// // 		return;
// // 	}

// // 	void *new_transfer_context()
// // 	{
// // 		return (void *)(new TransferContext());
// // 	}

// // 	void attach_process_to_transfer_context(void *ctx, void *processing)
// // 	{
// // 		auto ctx_ = (TransferContext *)ctx;
// // 		ctx_->processing = (Processing *)processing;
// // 		return;
// // 	}

// // 	void free_transfer_context(void *ctx)
// // 	{
// // 		delete (TransferContext *)ctx;
// // 		return;
// // 	}

// // 	void *new_device_to_buffer(void *device, void *buf, void *callback, void *ctx)
// // 	{
// // 		auto ret = new SapAcqDeviceToBuf((SapAcqDevice *)device, (SapBuffer *)buf, (SapXferCallback)callback, ctx);
// // 		if (!ret->Create())
// // 		{
// // 			throw std::runtime_error("Failed to create device to buffer object.");
// // 		}
// // 	}

// // 	void free_device_to_buffer(void *d2b)
// // 	{
// // 		((SapAcqDeviceToBuf *)d2b)->Destroy();
// // 		delete (SapAcqDeviceToBuf *)d2b;
// // 		return;
// // 	}

// // 	void set_device_buffer_autoempty(void *d2b, int val)
// // 	{
// // 		((SapAcqDeviceToBuf *)d2b)->SetAutoEmpty((bool)val);
// // 		return;
// // 	}

// // 	void *new_processing(void *buf, RGBFrameCallback f_callback, void *p_callback, void *ctx)
// // 	{
// // 		auto ret = new Processing((SapBuffer *)buf, f_callback, (SapProCallback)p_callback, ctx);
// // 		if (!ret->Create())
// // 		{
// // 			throw std::runtime_error("Failed to create processing object.");
// // 		}
// // 		return (void *)ret;
// // 	}

// // 	void free_processing(void *proc)
// // 	{
// // 		((Processing *)proc)->Destroy();
// // 		delete (Processing *)proc;
// // 		return;
// // 	}

// // 	void set_processing_autoempty(void *proc, int val)
// // 	{
// // 		((Processing *)proc)->SetAutoEmpty((bool)val);
// // 		return;
// // 	}

// // 	void free_rgb_frame(RGBFrame frame)
// // 	{
// // 		free(frame.data);
// // 		return;
// // 	}

// // 	SapLocationW sap_location_new(char *acq_server_name, unsigned int acq_device_number)
// // 	{
// // #ifdef _WIN32
// // 		return new SapLocation(acq_server_name, acq_device_number);
// // #else
// // 		return NULL;
// // #endif
// // 	}

// // 	int sap_manager_get_server_count()
// // 	{
// // #ifdef _WIN32
// // 		return SapManager::GetServerCount();
// // #else
// // 		return -1;
// // #endif
// // 	}

// // 	bool sap_manager_get_server_name_by_index(int i, char *server_name)
// // 	{
// // #ifdef _WIN32
// // 		return SapManager::GetServerName(i, server_name, 30);
// // #else
// // 		return false;
// // #endif
// // 	}

// // 	int sap_manager_get_server_index(char *acq_server_name)
// // 	{
// // #ifdef _WIN32
// // 		return SapManager::GetServerIndex(acq_server_name);
// // #else
// // 		return -1;
// // #endif
// // 	}

// // 	int sap_manager_get_resource_device_count(char *acq_server_name)
// // 	{
// // #ifdef _WIN32
// // 		return SapManager::GetResourceCount(acq_server_name, SapManager::ResourceAcq);
// // #else
// // 		return -1;
// // #endif
// // 	}

// // 	int sap_manager_get_resource_camera_count(char *acq_server_name)
// // 	{
// // #ifdef _WIN32
// // 		return SapManager::GetResourceCount(acq_server_name, SapManager::ResourceAcqDevice);
// // #else
// // 		return -1;
// // #endif
// // 	}

// // 	SapAcquisitionW sap_acquisition_new(SapLocationW loc, char *config_file_name)
// // 	{
// // #ifdef _WIN32
// // 		return new SapAcquisition(*(SapLocation *)loc, config_file_name);
// // #else
// // 		return NULL;
// // #endif
// // 	}

// // 	bool sap_acquisition_create(SapAcquisitionW acq)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcquisition *)acq)->Create();
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	SapBufferWithTrashW sap_buffer_with_trash_new(unsigned int count, SapAcquisitionW acq)
// // 	{
// // #ifdef _WIN32
// // 		return new SapBufferWithTrash(count, (SapAcquisition *)acq);
// // #else
// // 		return NULL;
// // #endif
// // 	}

// // 	bool sap_buffer_with_trash_create(SapBufferWithTrashW buf)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapBufferWithTrash *)buf)->Create();
// // #else
// // 		return false;
// // #endif
// // 	}

// // 	SapViewW sap_view_new(SapBufferWithTrashW buf)
// // 	{
// // #ifdef _WIN32
// // 		// return SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
// // 		// SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq)
// // 		// return SapAcquisition(*loc, configFilename)
// // 		return new SapView((SapBuffer *)buf, SapHwndAutomatic);
// // 		// return NULL;
// // #else
// // 		return NULL;
// // #endif
// // 	}

// // 	bool sap_view_create(SapViewW view)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapView *)view)->Create();
// // #else
// // 		return false;
// // #endif
// // 	}

// // 	bool sap_view_destroy(SapViewW view)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapView *)view)->Destroy();
// // #else
// // 		return false;
// // #endif
// // 	}

// // 	SapAcqToBufW sap_acq_to_buf_new(SapAcquisitionW acq, SapBufferWithTrashW buf, SapCallbackW callback, SapViewW view)
// // 	{
// // #ifdef _WIN32
// // 		// return new SapAcqToBuf((SapAcquisition *)acq, (SapBuffer *)buf, (SapXferCallback *)callback, (void *)view);
// // 		return new SapAcqToBuf((SapAcquisition *)acq, (SapBuffer *)buf, (SapXferCallback)callback, (void *)view);
// // #else
// // 		return NULL;
// // #endif
// // 	}

// // 	bool sap_acq_to_buf_destroy(SapAcqToBufW sat)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcqToBuf *)sat)->Destroy();
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	bool sap_acq_to_buf_create(SapAcqToBufW sat)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcqToBuf *)sat)->Create();
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	bool sap_acq_to_buf_grab(SapAcqToBufW sat)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcqToBuf *)sat)->Grab();
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	bool sap_acq_to_buf_freeze(SapAcqToBufW sat)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcqToBuf *)sat)->Freeze();
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	bool sap_acq_to_buf_unresigter_callback(SapAcqToBufW sat)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcqToBuf *)sat)->UnregisterCallback();
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	bool sap_acq_to_buf_wait(SapAcqToBufW sat, int duration)
// // 	{
// // #ifdef _WIN32
// // 		return ((SapAcqToBuf *)sat)->Wait(duration);
// // #else
// // 		return false;
// // #endif
// // 	}
// // 	// void dump_callback(void *info)
// // 	// {
// // 	// 	auto a = (SapXferCallbackInfo *)info;
// // 	// 	auto v = (SapView *)a.
// // 	// }
// // }