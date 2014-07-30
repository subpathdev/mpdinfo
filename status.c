#include <mpd/status.h>
#include <mpd/song.h>
#include <mpd/client.h>
#include <mpd/tag.h>

#include "debug.h"
#include "mpdinfo.h"
#include "parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int getStatusStruct(struct mpd_status **mpdstatus) {

	struct mpd_connection* conn = NULL;
	getConnection(&conn);
        *mpdstatus = mpd_run_status(conn);
        if (!*mpdstatus) {
                debug("FAIL", mpd_connection_get_error_message(conn));
                return 0;
        }
        return 1;
}

int getDBUpdateStatus() {

	struct mpd_status *mpdstatus = NULL;

        if (!getStatusStruct(&mpdstatus)) {
                return -1;
        }
	
	int update = mpd_status_get_update_id(mpdstatus);

	mpd_status_free(mpdstatus);

	if (update != 0) {
		return 1;
	}
	return 0;
}

char* getDBUpdateString() {

	char* token = NULL;
	int update = getDBUpdateStatus();
	char* str_update = NULL;

	if (update) {
		token = getTokenByStatus(C_TOKEN_DBUPDATE);
	} else {
		token = getNoneToken(C_TOKEN_DBUPDATE);
	}
	str_update = malloc(strlen(token) + 1);
	strcpy(str_update, token);
	free(token);
	return str_update;
}

int getRepeatStatus() {
        struct mpd_status *mpdstatus = NULL;

        if (!getStatusStruct(&mpdstatus)) {
                return -1;
        }

        int repeat = mpd_status_get_repeat(mpdstatus);

        mpd_status_free(mpdstatus);

        return repeat;


}

char* getRepeatString() {

	char* token = NULL;
        int status = getRepeatStatus();

	char* str_status = NULL;

        if (status) {
		token = getTokenByStatus(C_TOKEN_REPEAT);
        } else {
                token = getNoneToken(C_TOKEN_REPEAT);
        }
	str_status = malloc(strlen(token) + 1);
	strcpy(str_status, token);
	free(token);
	return str_status;
}

int getStatus() {
        struct mpd_status *mpdstatus = NULL;

        if (!getStatusStruct(&mpdstatus)) {
                return -1;
        }
        int status = mpd_status_get_state(mpdstatus);

        mpd_status_free(mpdstatus);

        return status;
}

char* getStatusString() {

        char* output = malloc(8);
        int status = getStatus();

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

char* getTitle() {
        
	struct mpd_connection* conn = NULL;

	getConnection(&conn);

	struct mpd_song *song = mpd_run_current_song(conn);

        if (song == NULL) {
                return "";
        }

        const char* tit = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        
        if (tit == NULL) {
                return "";
        }
        
        if (strcmp(tit, "") == 0) {
                return "";
        }

        char* title = malloc(strlen(tit) +1);
        strcpy(title, tit);
        mpd_song_free(song);

        return title;
}

char* getArtist() {
	struct mpd_connection* conn = NULL;

	getConnection(&conn);

        struct mpd_song *song = mpd_run_current_song(conn);

	if (song == NULL) {
		return "";
	}

	const char* art = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
	
	if (art == NULL) {
		return "";
	}
        if (strcmp(art,"") == 0) {
		return "";
	}

	char* artist = malloc(strlen(art) +1);
        strcpy(artist, art);
        mpd_song_free(song);
	return artist;	
}

int getVolume() {
        struct mpd_status* status = NULL;
	if(!getStatusStruct(&status)) {
		return -1;
	}
	int volume = mpd_status_get_volume(status);
        mpd_status_free(status);
        return volume;
}

char* getVolumeString() {
        int vol = getVolume();
        char* volString = malloc(4);
        sprintf(volString, "%d", vol);
        return volString;
}
