#pragma once

#include <map>


namespace msg
{

	// ------------------------------------
	// Global Message Dispatcher
	// ------------------------------------
	class Dispatcher;
	extern Dispatcher gDispatcher;
	// ------------------------------------

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
		, SHOW_LIVE_SKELETON
		, HIDE_LIVE_SKELETON
		// Skeleton recording controls
		, START_SKELETON_RECORDING
		, STOP_SKELETON_RECORDING
		, CLEAR_SKELETON_RECORDING
		, SAVE_SKELETON_RECORDING
		, LOAD_SKELETON_RECORDING
		, SET_RECORDING_LABEL
		// Skeleton playback controls
		, START_PLAYBACK
		, STOP_PLAYBACK
		, PLAYBACK_FIRST_FRAME
		, PLAYBACK_LAST_FRAME
		, PLAYBACK_PREV_FRAME
		, PLAYBACK_NEXT_FRAME
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
	class SetRecordingLabelMessage : public Message
	{
	public:
		SetRecordingLabelMessage(const std::string& text)
			: Message(SET_RECORDING_LABEL)
			, text(text)
		{}
		const std::string text;
	};
	// ------------------------------------------------------------------------
	class ShowLiveSkeletonMessage : public Message
	{
	public: ShowLiveSkeletonMessage() : Message(SHOW_LIVE_SKELETON) {}
	};
	// ------------------------------------------------------------------------
	class HideLiveSkeletonMessage : public Message
	{
	public: HideLiveSkeletonMessage() : Message(HIDE_LIVE_SKELETON) {}
	};
	// ------------------------------------------------------------------------
	class PlaybackFirstFrameMessage : public Message
	{
	public: PlaybackFirstFrameMessage() : Message(PLAYBACK_FIRST_FRAME) {}
	};
	// ------------------------------------------------------------------------
	class PlaybackLastFrameMessage : public Message
	{
	public: PlaybackLastFrameMessage() : Message(PLAYBACK_LAST_FRAME) {}
	};
	// ------------------------------------------------------------------------
	class PlaybackNextFrameMessage : public Message
	{
	public: PlaybackNextFrameMessage() : Message(PLAYBACK_NEXT_FRAME) {}
	};
	// ------------------------------------------------------------------------
	class PlaybackPrevFrameMessage : public Message
	{
	public: PlaybackPrevFrameMessage() : Message(PLAYBACK_PREV_FRAME) {}
	};
	// ------------------------------------------------------------------------
	//class Message : public Message
	//{
	//public: Message() : Message() {}
	//};


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
		virtual void process(const SetRecordingLabelMessage *message) {}
		virtual void process(const ShowLiveSkeletonMessage  *message) {}
		virtual void process(const HideLiveSkeletonMessage  *message) {}
		virtual void process(const PlaybackFirstFrameMessage *message) {}
		virtual void process(const PlaybackLastFrameMessage  *message) {}
		virtual void process(const PlaybackNextFrameMessage  *message) {}
		virtual void process(const PlaybackPrevFrameMessage  *message) {}
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

} // end namespace Messages
