#include <iostream>
#include <atomic>
#include <memory>
#include "SapClassBasic.h"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

class SapMyProcessing : public SapProcessing
{
public:
	SapMyProcessing(SapBuffer *pBuffers, SapProCallback pCallback, void *pContext);
	virtual ~SapMyProcessing();

protected:
	virtual BOOL Run();
};

SapMyProcessing::SapMyProcessing(SapBuffer *pBuffers, SapProCallback pCallback, void *pContext)
	: SapProcessing(pBuffers, pCallback, pContext)
{
}

SapMyProcessing::~SapMyProcessing()
{
	if (m_bInitOK)
		Destroy();
}

BOOL SapMyProcessing::Run()
{
	// Get the current buffer index
	const int proIndex = GetIndex();

	// If this is not true, buffer has overflown
	SapBuffer::State state;
	bool goodContent = m_pBuffers->GetState(proIndex, &state) && state == SapBuffer::StateFull;

	std::cout << "grab => status=" << goodContent << ", proIndex=" << proIndex << std::endl;

	if (!goodContent)
	{
		void *inAddress = nullptr;
		m_pBuffers->GetAddress(proIndex, &inAddress);
		int inSize = 0;
		m_pBuffers->GetSpaceUsed(proIndex, &inSize);

		// Width, height and pixel format are received from the camera
		const int width = m_pBuffers->GetWidth();
		const int height = m_pBuffers->GetHeight();
		const auto format = m_pBuffers->GetFormat();
		const int outSize = width * height;

		std::cout << width << "x" << height << std::endl;

		// Skip unexpected pixel format or incomplete frame
		goodContent = format == SapFormatMono8 && inSize == outSize;

		if (!goodContent)
		{
			// Copy data to vector
			std::vector<uint8_t> outBuffer(outSize);
			std::copy((uint8_t *)inAddress, (uint8_t *)(inAddress) + outSize, outBuffer.begin());

			// Print the first line
			for (int i = 0; i < width; i++)
			{
				std::cout << std::hex << int(outBuffer[i]);
			}
			std::cout << std::endl
					  << std::endl;
		}
	}

	return TRUE;
}

// Information to pass to callbacks
struct TransferContext
{
	std::atomic_int frameGrabCount = 0, frameProcessingCount = 0;
	SapMyProcessing *processing;
};

void transferCallback(SapXferCallbackInfo *info)
{
	auto context = (TransferContext *)info->GetContext();
	context->frameGrabCount++;
	if (!info->IsTrash())
	{
		// Execute Run() for this frame
		context->processing->ExecuteNext();
	}
}

// Processing callback is called after Run()
void processingCallback(SapProCallbackInfo *info)
{
	auto context = (TransferContext *)info->GetContext();

	// Processing has finished
	context->frameProcessingCount++;
}

static void XferCallback(SapXferCallbackInfo *pInfo)
{
	SapView *pView = (SapView *)pInfo->GetContext();
	SapBuffer *pBuffer = pView->GetBuffer();
	// refresh view
	// pView->Show();
	printf("h %d,w %d,f %d\n", pBuffer->GetHeight(), pBuffer->GetWidth(), pBuffer->GetFormat());
	// refresh framerate
	static float lastframerate = 0.0f;
	SapTransfer *pXfer = pInfo->GetTransfer();

	if (pXfer->UpdateFrameRateStatistics())
	{
		SapXferFrameRateInfo *pFrameRateInfo = pXfer->GetFrameRateStatistics();
		float framerate = 0.0f;

		if (pFrameRateInfo->IsLiveFrameRateAvailable())
			framerate = pFrameRateInfo->GetLiveFrameRate();

		// check if frame rate is stalled
		if (pFrameRateInfo->IsLiveFrameRateStalled())
		{
			printf("Live frame rate is stalled.\n");
		}
		// update FPS only if the value changed by +/- 0.1
		else if ((framerate > 0.0f) && (abs(lastframerate - framerate) > 0.1f))
		{
			printf("Grabbing at %.1f frames/sec\n", framerate);
			lastframerate = framerate;
		}
	}
}

void grab()
{
	auto location = SapLocation("Xtium-CL_MX4_1", 1);
	std::cout << "create location" << std::endl;

	auto acq = SapAcquisition(location, "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_cct_Default_Default.ccf");
	std::cout << "create acq" << std::endl;
	if (acq.Create())
		std::cout << "acq.create" << std::endl;

	auto buffers = SapBufferWithTrash(2, &acq);
	std::cout << "create buffers" << std::endl;
	if (buffers.Create())
		std::cout << "buffers.create" << std::endl;

	TransferContext ctx = {0};
	std::cout << "create ctx" << std::endl;

	auto proc = SapMyProcessing(&buffers, processingCallback, &ctx);
	std::cout << "create proc" << std::endl;
	if (proc.Create())
		std::cout << "proc.create" << std::endl;

	ctx.processing = &proc;

	auto acq_to_buffers = SapAcqToBuf(&acq, &buffers, transferCallback, &ctx);
	std::cout << "create acq_to_buffers" << std::endl;
	if (acq_to_buffers.Create())
		std::cout << "acq_to_buffers.create" << std::endl;

	if (acq_to_buffers.Grab())
		std::cout << "acq_to_buffers.grab" << std::endl;

	std::cout << "Press any key to stop grab" << std::endl;
	// CorGetch();
	std::this_thread::sleep_for(100s);

	if (acq_to_buffers.Freeze())
		std::cout
			<< "acq_to_buffers.freeze" << std::endl;

	// Number of frames to receive from the camera
	// const int maxFrameCount = 10;

	// TransferContext context;

	// auto camera = getDeviceBySN(serialNumber);
	// auto loc = SapLocation("Xtium-CL_MX4_1", 1);
	// auto acq = SapAcquisition(loc, "C://Program Files//Teledyne DALSA//Sapera//CamFiles//User//b_cct_Default_Default.ccf");
	// std::unique_ptr<SapBuffer> buffer = std::make_unique<SapBufferWithTrash>(maxFrameCount, &acq, SapDefBufferType);
	// std::cout << "create sap buffer:" << buffer.get() << "haha" << std::endl;
	// std::unique_ptr<SapTransfer> transfer = std::make_unique<SapAcqToBuf>(&acq, buffer.get(), nullptr, &context);
	// std::cout << "create sap transfer" << std::endl;

	// context.processing = std::make_shared<SapMyProcessing>(buffer.get(), processingCallback, &context);
	// std::cout << "create my processing" << std::endl;

	// auto cleanup = [&]()
	// {
	// 	if (context.processing)
	// 		context.processing->Destroy();
	// 	if (transfer)
	// 		transfer->Destroy();
	// 	if (buffer)
	// 		buffer->Destroy();
	// 	acq.Destroy();
	// };

	// try
	// {
	// 	if (!buffer->Create())
	// 	{
	// 		std::cerr << "failed to create buffer object" << std::endl;
	// 		throw std::runtime_error("Failed to create buffer object.");
	// 	}

	// 	if (!transfer->Create())
	// 	{
	// 		std::cerr << "failed to create transfer" << std::endl;
	// 		throw std::runtime_error("Failed to create transfer object.");
	// 	}
	// 	if (!context.processing->Create())
	// 	{
	// 		std::cerr << "failed to create processing object" << std::endl;
	// 		throw std::runtime_error("Failed to create processing object.");
	// 	}
	// 	transfer->SetAutoEmpty(false);
	// 	std::cout << "set transfer auto emtpy false" << std::endl;
	// 	context.processing->SetAutoEmpty(true);
	// 	std::cout << "set processing auto empty true" << std::endl;
	// 	context.processing->Init();
	// 	std::cout << "init processing" << std::endl;

	// 	transfer->Grab();
	// 	std::cout << "grabing transfer" << std::endl;

	// 	// Wait for the camera to grab all frames
	// 	while (context.frameGrabCount < maxFrameCount)
	// 		;

	// 	transfer->Freeze();
	// 	if (!transfer->Wait(5000))
	// 	{
	// 		throw std::runtime_error("Failed to stop grab.");
	// 	}

	// 	// Wait for processing to complete
	// 	while (context.frameProcessingCount < maxFrameCount)
	// 		;

	// 	cleanup();
	// }
	// catch (...)
	// {
	// 	cleanup();
	// 	throw;
	// }
}

int main(int argc, char **argv)
{
	grab();
	return 0;
}