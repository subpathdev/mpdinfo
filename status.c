#include <mpd/status.h>
#include <mpd/song.h>
#include <mpd/tag.h>

#include "mpdinfo.h"
//#include "parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int getStatusByFunc(Config* config, void* func) {

	if (!config->mpd_status) {
		return -1;
	}
	int (*f)(struct mpd_status* status) = func;
	int status = f(config->mpd_status);

	return status;
}

int getDBUpdateStatus(Config* config, int status) {

	int update = getStatusByFunc(config, &mpd_status_get_update_id);

	if (update != 0) {
		return 1;
	}
	return 0;
}

int getRandomStatus(Config* config, int status) {
	return getStatusByFunc(config, &mpd_status_get_random);
}

int getRepeatStatus(Config* config, int status) {
        return getStatusByFunc(config, &mpd_status_get_repeat);
}

int getStatus(Config* config, int status) {
        return getStatusByFunc(config, &mpd_status_get_state);
}

char* getTokenStatusString(int status, int playStatus, TokenConfigItem* item) {

	char* output = NULL;

	if (status) {
		switch	(playStatus) {
			case MPD_STATE_STOP:
				output = item->stop;
				break;
			case MPD_STATE_PAUSE:
				output = item->pause;
				break;
			case MPD_STATE_PLAY:
				output = item->play;
				break;
			default:
				output = item->none;
				break;
		}

	} else {
		output = item->off;
	}

	// check for NULL pointer
	char* str_status;
	if (!output) {
		str_status = malloc(1);
		str_status[0] = 0;
	} else {
		str_status = malloc(strlen(output) + 1);
		strcpy(str_status, output);
	}
	return str_status;
}

char* getRepeatString(Config* config, int status) {
	return getTokenStatusString(getRepeatStatus(config, status), status, config->tokens->repeat);
}

char* getRandomString(Config* config, int status) {
	return getTokenStatusString(getRandomStatus(config, status), status, config->tokens->random);
}
char* getDBUpdateString(Config* config, int status) {
	return getTokenStatusString(getDBUpdateStatus(config, status), status, config->tokens->dbupdate);
}

char* getStatusString(Config* config, int status) {

        char* output = malloc(8);

        switch (status) {

                case MPD_STATE_PLAY:
                        strcpy(output,"playing");
                        return output;
                case MPD_STATE_PAUSE:
                        strcpy(output,"pause");
                        return output;
                case MPD_STATE_STOP:
                        strcpy(output,"stopped");
                        return output;
                default:
                        strcpy(output,"unkown");
                        return output;
        }
}

char* getId3Tag(Config* config, int status, enum mpd_tag_type tag_type) {
	char* out = malloc(1);
	out[0] = 0;

	if (!config->curr_song) {
		return out;
	}

	const char* tag = mpd_song_get_tag(config->curr_song, tag_type, 0);
	
	if (tag == NULL || !strcmp(tag, "")) {
		return out;
	}
	
	free(out);
	
	out = malloc(strlen(tag) +1);
        strncpy(out, tag, strlen(tag) + 1);
	return out;

}
char* getTitle(Config* config, int status) {
        return getId3Tag(config, status, MPD_TAG_TITLE);
}
char* getArtist(Config* config, int status) {
	return getId3Tag(config, status, MPD_TAG_ARTIST);
}

char* getFilename(Config* config, int status) {
	char* out = malloc(1);
	out[0] = 0;

	if (!config->curr_song) {
		return out;
	}

	const char* uri = mpd_song_get_uri(config->curr_song);

	if (uri == NULL) {
		return out;
	}

	if (strcmp(uri, "") == 0) {
		return out;
	}

	const char* str = strrchr(uri, '/');

	// check if more then the / is in the string
	if (!str || strlen(str) < 2) {
		str = uri;
	}

	str++;

	free(out);
	out = malloc(strlen(str) + 1);
	strncpy(out, str, strlen(str) + 1);
	
	return out;
}

char* getElapsedTime(Config* config, int status) {

	char* out = malloc(1);
	out[0] = 0;
	
	if (!config->mpd_status) {
		return out;
	}

	unsigned time = mpd_status_get_elapsed_time(config->mpd_status);

	unsigned sec;
	unsigned min;

	sec = time % 60;
	min = time / 60;

	free(out);

	int length = snprintf(NULL, 0, "%d:%02d", min, sec) + 1;
	out = malloc(length);

	snprintf(out, length, "%d:%02d", min, sec);

	return out; 
}

int getVolume(Config* config, int status) {
	if(!config->mpd_status) {
		return -1;
	}
	int volume = mpd_status_get_volume(config->mpd_status);
        return volume;
}

char* getVolumeString(Config* config, int status) {
        int vol = getVolume(config, status);

	unsigned length = snprintf(NULL, 0, "%d", vol) + 1;
        char* volString = malloc(length);
        snprintf(volString, length, "%d", vol);
        return volString;
}

