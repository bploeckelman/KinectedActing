#pragma once

#include <map>


namespace msg
{

	// ------------------------------------------------------------------------
	// Message Type Enumeration -----------------------------------------------
	// ------------------------------------------------------------------------
	enum EType
	{
		// General application
		  QUIT_PROGRAM
		// Kinect device
		, START_KINECT_DEVICE
		, STOP_KINECT_DEVICE
		, SET_KINECT_DEVICE_ID_LABEL
		// Skeleton recording controls
		, START_SKELETON_RECORDING
		, STOP_SKELETON_RECORDING
		, CLEAR_SKELETON_RECORDING
		, SAVE_SKELETON_RECORDING
		, LOAD_SKELETON_RECORDING
		// Skeleton playback controls
		, START_PLAYBACK
		, STOP_PLAYBACK
		, FIRST_FRAME_PLAYBACK
		, LAST_FRAME_PLAYBACK
		, PREVIOUS_FRAME_PLAYBACK
		, NEXT_FRAME_PLAYBACK
	};

	// ------------------------------------------------------------------------
	// Base Message Class -----------------------------------------------------
	// ------------------------------------------------------------------------

	class Message
	{
	public:
		explicit Message(EType type) : _type(type) {}
		const EType& type() const { return _type; }

	private:
		const EType _type;
	};

	// ------------------------------------------------------------------------
	// Message Types ----------------------------------------------------------
	// ------------------------------------------------------------------------
	class QuitProgramMessage : public Message
	{
	public: QuitProgramMessage() : Message(QUIT_PROGRAM) {}
	};
	// ------------------------------------------------------------------------
	class StartKinectDeviceMessage : public Message
	{
	public: StartKinectDeviceMessage() : Message(START_KINECT_DEVICE) {}
	};
	// ------------------------------------------------------------------------
	class StopKinectDeviceMessage : public Message
	{
	public: StopKinectDeviceMessage() : Message(STOP_KINECT_DEVICE) {}
	};
	// ------------------------------------------------------------------------
	class StartRecordingMessage : public Message
	{
	public: StartRecordingMessage() : Message(START_SKELETON_RECORDING) {}
	};
	// ------------------------------------------------------------------------
	class StopRecordingMessage : public Message
	{
	public: StopRecordingMessage() : Message(STOP_SKELETON_RECORDING) {}
	};
	// ------------------------------------------------------------------------
	class ClearRecordingMessage : public Message
	{
	public: ClearRecordingMessage() : Message(CLEAR_SKELETON_RECORDING) {}
	};


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------


	class Handler
	{
	public:
		// Overload the specific Message subclasses to be handled
		virtual void process(const QuitProgramMessage       *message) {}
		virtual void process(const StartKinectDeviceMessage *message) {}
		virtual void process(const StopKinectDeviceMessage  *message) {}
		virtual void process(const StartRecordingMessage    *message) {}
		virtual void process(const StopRecordingMessage     *message) {}
		virtual void process(const ClearRecordingMessage    *message) {}
	};


	// ------------------------------------------------------------------------


	class Dispatcher
	{
	public:
		void registerHandler(EType type, Handler* handler)
		{
			handlers.insert(std::make_pair(type, handler));
		}

		template<class MsgType> inline void dispatchMessage(const MsgType& msg)
		{
			auto range = handlers.equal_range(msg.type());

			for (auto it = range.first; it != range.second; ++it) {
				Handler *handler = (*it).second;
				handler->process(&msg);
			}
		}

	private:
		std::multimap<EType, Handler*> handlers;
	};


	// ------------------------------------
	// Global Message Dispatcher
	// ------------------------------------
	extern Dispatcher gDispatcher;
	// ------------------------------------

} // end namespace Messages
