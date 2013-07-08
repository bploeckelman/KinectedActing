#pragma once

#include <map>


class Message
{
public:
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

	explicit Message(EType type) : _type(type) {}

	const EType& type() const { return _type; }

protected:
	const EType _type;

};


// ----------------------------------------------------------------------------


class QuitProgramMessage : public Message
{
public:
	QuitProgramMessage() : Message(QUIT_PROGRAM) {}
};

// ----------------------------------------------------------------------------

class StartKinectDeviceMessage : public Message
{
public:
	StartKinectDeviceMessage() : Message(START_KINECT_DEVICE) {}
};

// ----------------------------------------------------------------------------

class StopKinectDeviceMessage : public Message
{
public:
	StopKinectDeviceMessage() : Message(STOP_KINECT_DEVICE) {}
};

// ----------------------------------------------------------------------------

class StartRecordingMessage : public Message
{
public:
	StartRecordingMessage() : Message(START_SKELETON_RECORDING) {}
};

// ----------------------------------------------------------------------------

class StopRecordingMessage : public Message
{
public:
	StopRecordingMessage() : Message(STOP_SKELETON_RECORDING) {}
};

// ----------------------------------------------------------------------------

class ClearRecordingMessage : public Message
{
public:
	ClearRecordingMessage() : Message(CLEAR_SKELETON_RECORDING) {}
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


class MessageHandler
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


// ----------------------------------------------------------------------------


class MessageDispatcher
{
public:
	void registerHandler(Message::EType type, MessageHandler* handler);

	template<class MsgType> void dispatchMessage(const MsgType& message);

private:
	std::multimap<Message::EType, MessageHandler*> handlers;

};

template<class MsgType>
inline void MessageDispatcher::dispatchMessage(const MsgType& message)
{
	auto handlersRange = handlers.equal_range(message.type());
	for (auto it = handlersRange.first; it != handlersRange.second; ++it) {
		MessageHandler *handler = (*it).second;
		handler->process(&message);
	}
}

extern MessageDispatcher gMessageDispatcher;
