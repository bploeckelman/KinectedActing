#pragma once

#include "Animation/AnimationTypes.h"

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
		, TOGGLE_SEATED_MODE
		// Skeleton recording controls
		, START_SKELETON_RECORDING
		, STOP_SKELETON_RECORDING
		, CLEAR_SKELETON_RECORDING
		, SAVE_SKELETON_RECORDING
		, LOAD_SKELETON_RECORDING
		, EXPORT_SKELETON_BVH
		, SET_RECORDING_LABEL
		// Skeleton playback controls
		, PLAYBACK_START
		, PLAYBACK_STOP
		, PLAYBACK_FIRST_FRAME
		, PLAYBACK_LAST_FRAME
		, PLAYBACK_PREV_FRAME
		, PLAYBACK_NEXT_FRAME
		, PLAYBACK_SET_DELTA
		, PLAYBACK_SET_PROGRESS
		// Layering controls
		, START_LAYERING
		, LAYER_SELECT
		, ADD_LAYER_ITEM
		, MAPPING_MODE_SELECT
		// Misc
		, SET_INFO_LABEL
		, SHOW_BONE_PATH
		, HIDE_BONE_PATH
		, UPDATE_BONE_MASK
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
	class ExportSkeletonBVHMessage : public Message
	{
	public: ExportSkeletonBVHMessage() : Message(EXPORT_SKELETON_BVH) {}
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
	class ToggleSeatedModeMessage : public Message
	{
	public: ToggleSeatedModeMessage() : Message(TOGGLE_SEATED_MODE) {}
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
	class PlaybackStartMessage : public Message
	{
	public: PlaybackStartMessage() : Message(PLAYBACK_START) {}
	};
	// ------------------------------------------------------------------------
	class PlaybackStopMessage : public Message
	{
	public: PlaybackStopMessage() : Message(PLAYBACK_STOP) {}
	};
	// ------------------------------------------------------------------------
	class PlaybackSetDeltaMessage : public Message
	{
	public:
		PlaybackSetDeltaMessage(const float delta)
			: Message(PLAYBACK_SET_DELTA)
			, delta(delta)
		{}
		const float delta;
	};
	// ------------------------------------------------------------------------
	class PlaybackSetProgressMessage : public Message
	{
	public:
		PlaybackSetProgressMessage(const float progress)
			: Message(PLAYBACK_SET_PROGRESS)
			, progress(progress)
		{}
		const float progress;
	};
	// ------------------------------------------------------------------------
	class StartLayeringMessage : public Message
	{
	public: StartLayeringMessage() : Message(START_LAYERING) {}
	};
	// ------------------------------------------------------------------------
	class LayerSelectMessage: public Message
	{
	public:
		LayerSelectMessage(const std::string& layerName)
			: Message(LAYER_SELECT)
			, layerName(layerName)
		{}
		const std::string layerName;
	};
	// ------------------------------------------------------------------------
	class MappingModeSelectMessage : public Message
	{
	public:
		MappingModeSelectMessage(const unsigned int mode)
			: Message(MAPPING_MODE_SELECT)
			, mode(mode)
		{}
		const unsigned int mode;
	};
	// ------------------------------------------------------------------------
	class SetInfoLabelMessage : public Message
	{
	public:
		SetInfoLabelMessage(const std::string& text)
			: Message(SET_INFO_LABEL)
			, text(text)
		{}
		const std::string text;
	};
	// ------------------------------------------------------------------------
	class AddLayerItemMessage: public Message
	{
	public:
		AddLayerItemMessage(const std::string& item)
			: Message(ADD_LAYER_ITEM)
			, item(item)
		{}
		const std::string item;
	};
	// ------------------------------------------------------------------------
	class ShowBonePathMessage : public Message
	{
	public: ShowBonePathMessage() : Message(SHOW_BONE_PATH) {}
	};
	// ------------------------------------------------------------------------
	class HideBonePathMessage : public Message
	{
	public: HideBonePathMessage() : Message(HIDE_BONE_PATH) {}
	};
	// ------------------------------------------------------------------------
	class UpdateBoneMaskMessage : public Message
	{
	public:
		UpdateBoneMaskMessage(const BoneMask& boneMask)
			: Message(UPDATE_BONE_MASK)
			, boneMask(boneMask)
		{}
		const BoneMask boneMask;
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
		virtual void process(const ExportSkeletonBVHMessage *message) {}
		virtual void process(const SetRecordingLabelMessage *message) {}
		virtual void process(const ShowLiveSkeletonMessage  *message) {}
		virtual void process(const HideLiveSkeletonMessage  *message) {}
		virtual void process(const ToggleSeatedModeMessage  *message) {}
		virtual void process(const PlaybackFirstFrameMessage *message) {}
		virtual void process(const PlaybackLastFrameMessage  *message) {}
		virtual void process(const PlaybackNextFrameMessage  *message) {}
		virtual void process(const PlaybackPrevFrameMessage  *message) {}
		virtual void process(const PlaybackStartMessage      *message) {}
		virtual void process(const PlaybackStopMessage       *message) {}
		virtual void process(const PlaybackSetDeltaMessage   *message) {}
		virtual void process(const PlaybackSetProgressMessage *message) {}
		virtual void process(const StartLayeringMessage       *message) {}
		virtual void process(const LayerSelectMessage         *message) {}
		virtual void process(const AddLayerItemMessage        *message) {}
		virtual void process(const MappingModeSelectMessage   *message) {}
		virtual void process(const SetInfoLabelMessage        *message) {}
		virtual void process(const ShowBonePathMessage        *message) {}
		virtual void process(const HideBonePathMessage        *message) {}
		virtual void process(const UpdateBoneMaskMessage      *message) {}
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
