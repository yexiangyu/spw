#include "spw.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

// #include "spw.hpp"

#ifdef _WIN64
#include "SapClassBasic.h"

unsigned long long epoch_now()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

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

struct Context_
{
	std::atomic_int frame_grab_count = 0;
	std::atomic_int frame_proc_count = 0;
	Processing_ *processing;
};

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

	if (state == SapBuffer::StateFull || state == SapBuffer::StateEmpty)
	{
		void *addr = nullptr;
		int mem_size = 0;
		m_pBuffers->GetAddress(buf_idx, &addr);
		m_pBuffers->GetSpaceUsed(buf_idx, &mem_size);
		int w = m_pBuffers->GetWidth();
		int h = m_pBuffers->GetHeight();
		int c = 0;
		if (w * h)
		{
			c = mem_size / (w * h);
		}

		// int frame_id = ctx->frame_grab_count;

		// std::cout << "current buff index=" << buf_idx << ",state=" << state << ",addr=" << addr << ",mem_size=" << mem_size << ",size=" << w << "x" << h << "x" << c << std::endl;

		if (f_callback)
		{
			unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * mem_size);
			// memset((void *)data, 255, mem_size);
			std::copy((unsigned char *)addr, (unsigned char *)(addr) + mem_size, data);
			Frame frame = {static_cast<uint64_t>(w), static_cast<uint64_t>(h), static_cast<uint64_t>(c), static_cast<uint64_t>(ctx->frame_grab_count), epoch_now(), data};
			// std::cout << "<<<<<";
			// for (int i = 0; i < 20; i++)
			// {
			// 	std::cout << (int)(frame.data[i]) << "|";
			// }
			// std::cout << std::endl;
			// printf("%lx\n", f_callback);
			f_callback(&frame);
			free(frame.data);
		}
	}
	else
	{
		std::cout << "current buff index=" << buf_idx << ",state=" << state << std::endl;
	}

	return true;
}

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
	DLL Location location_new(const char *server_name, int device_id)
	{
		// printf("%s\n", server_name);
		return new SapLocation(server_name, device_id);
	}

	DLL bool location_free(Location location)
	{
		return free_as<SapLocation>(location);
	}

	DLL Acq acq_new(Location location, const char *config_file_name)
	{
		// printf("%s\n", config_file_name);
		return new SapAcquisition(*(SapLocation *)location, config_file_name);
	}

	DLL bool acq_create(Acq acq)
	{
		return create_as<SapAcquisition>(acq);
	}

	DLL bool acq_free(Acq acq)
	{
		return free_as<SapAcquisition>(acq);
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

	DLL void context_counter_reset(Context ctx)
	{
		((Context_ *)ctx)->frame_proc_count = 0;
		((Context_ *)ctx)->frame_grab_count = 0;
		return;
	}

	DLL bool processing_free(Processing proc)
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

	DLL void sleep_for_1s()
	{
		std::this_thread::sleep_for(1s);
	}
}

#endif