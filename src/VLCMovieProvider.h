/*
 * ImageSequenceProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef VLCMOVIEPROVIDER_H_
#define VLCMOVIEPROVIDER_H_

#include <string>
#include <vector>
#include <deque>

#include <vlc/vlc.h>

#include "ImageProvider.h"

namespace orka {

typedef uint32_t vlc_int;

class VLCMovieProvider: public ImageProvider {
Q_OBJECT
public:
    VLCMovieProvider(const std::string & filename);
    virtual ~VLCMovieProvider();
    void display(void *id);
    void lock(void ** p_pixels);
    void unlock(void *id, void * const *p_pixels);
private slots:
    void start();
    void stop();
    void toggleStartStop();
    void jog(int dframes);

private:
    vlc_int * frameData_;
    OrkaImage * currentFrame_;
    std::string filename_;
    bool playing_;
    int width_, height_, channels_;

    // VLC pointers
    libvlc_instance_t * vlc_instance_;
    libvlc_media_player_t * vlc_media_player_;
    libvlc_media_t * vlc_media_;
};

} /* namespace orka */
#endif /* VLCMOVIEPROVIDER_H_ */
