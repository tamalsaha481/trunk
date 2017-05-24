#include <stdio.h>
#include <gst/gst.h>  

/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData {
	GstElement *playbin2;  /* Our one and only element */
	gboolean playing;      /* Are we in the PLAYING state? */
	gboolean terminate;    /* Should we terminate execution? */
	gboolean seek_enabled; /* Is seeking enabled for this media? */
	gboolean seek_done;    /* Have we performed the seek already? */
	gint64   seek_start = -1;
	gint64   seek_end = -1;
	gint64 duration;       /* How long does this media last, in nanoseconds */

} CustomData;

/* Forward definition of the message processing function */
static void handle_message(CustomData *data, GstMessage *msg);

int main(int argc, char *argv[]) {
	CustomData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	data.playing = FALSE;
	data.terminate = FALSE;
	data.seek_enabled = FALSE;
	data.seek_done = FALSE;
	data.duration = GST_CLOCK_TIME_NONE;

	/* Initialize GStreamer */
	gst_init(&argc, &argv);

	/* Create the elements */
	data.playbin2 = gst_element_factory_make("playbin3", "playbin3");

	if (!data.playbin2) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Set the URI to play */
	g_object_set(data.playbin2, "uri", "https://gstreamer.freedesktop.org/media/large/PSPGO_FINAL.MP4", NULL);

	
	/* Start playing */
	ret = gst_element_set_state(data.playbin2, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.playbin2);
		return -1;
	}

	/* Listen to the bus */
	bus = gst_element_get_bus(data.playbin2);
	do {
		msg = gst_bus_timed_pop_filtered(bus, 1100 * GST_MSECOND,
			GstMessageType(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION));

		/* Parse message */
		if (msg != NULL) {
			handle_message(&data, msg);
		}
		else {
			/* We got no message, this means the timeout expired */
			if (data.playing) {
				GstFormat fmt = GST_FORMAT_TIME;
				gint64 current = -1, seek_start_sec = -1, seek_to_sec = -1;

				/* Query the current position of the stream */
				if (!gst_element_query_position(data.playbin2, fmt, &current)) {
					g_printerr("Could not query current position.\n");
				}

				/* If we didn't know it yet, query the stream duration */
				if (!GST_CLOCK_TIME_IS_VALID(data.duration)) {
					if (!gst_element_query_duration(data.playbin2, fmt, &data.duration)) {
						g_printerr("Could not query current duration.\n");
					}
				}

				/* Print current position and total duration */
				g_print("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
					GST_TIME_ARGS(current), GST_TIME_ARGS(data.duration));

				/* If seeking is enabled, we have not done it yet, and the time is right, seek */
				seek_start_sec = 15;
				seek_to_sec = 60;
				if (data.seek_enabled && !data.seek_done &&
					seek_start_sec >= data.seek_start && seek_start_sec < data.seek_end &&
					seek_to_sec >= data.seek_start && seek_to_sec < data.seek_end &&
					current > seek_start_sec * GST_SECOND) {
					g_print("\nReached %d s, performing seek to %d s...\n", seek_start_sec, seek_to_sec);
					gst_element_seek_simple(data.playbin2, GST_FORMAT_TIME,
						GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), seek_to_sec * GST_SECOND);
					data.seek_done = TRUE;
				}
			}
		}
	} while (!data.terminate);

	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(data.playbin2, GST_STATE_NULL);
	gst_object_unref(data.playbin2);
	getchar();
	return 0;
}

static void handle_message(CustomData *data, GstMessage *msg) {
	GError *err;
	gchar *debug_info;

	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_ERROR:
		gst_message_parse_error(msg, &err, &debug_info);
		g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
		g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
		g_clear_error(&err);
		g_free(debug_info);
		data->terminate = TRUE;

		break;
	case GST_MESSAGE_EOS:
		g_print("End-Of-Stream reached.\n");
		data->terminate = TRUE;
		break;
	case GST_MESSAGE_DURATION:
		/* The duration has changed, mark the current one as invalid */
		data->duration = GST_CLOCK_TIME_NONE;
		break;
	case GST_MESSAGE_STATE_CHANGED: {
		GstState old_state, new_state, pending_state;
		gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
		if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin2)) {
			g_print("Pipeline state changed from %s to %s:\n",
				gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));

			/* Remember whether we are in the PLAYING state or not */
			data->playing = (new_state == GST_STATE_PLAYING);

			if (data->playing) {
				/* We just moved to PLAYING. Check if seeking is possible */
				GstQuery *query;
				query = gst_query_new_seeking(GST_FORMAT_TIME);
				if (gst_element_query(data->playbin2, query)) {
					gst_query_parse_seeking(query, NULL, &data->seek_enabled, &data->seek_start, &data->seek_end);
					if (data->seek_enabled) {
						g_print("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
							GST_TIME_ARGS(data->seek_start), GST_TIME_ARGS(data->seek_end));
					}
					else {
						g_print("Seeking is DISABLED for this stream.\n");
					}
				}
				else {
					g_printerr("Seeking query failed.");
				}
				gst_query_unref(query);
			}
		}
	} break;
	default:
		/* We should not reach here */
		g_printerr("Unexpected message received.\n");
		break;
	}
	gst_message_unref(msg);
}