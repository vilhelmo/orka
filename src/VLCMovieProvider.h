/*
 * ImageSequenceProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef SRC_VLCMOVIEPROVIDER_H_
#define SRC_VLCMOVIEPROVIDER_H_

#include <vlc/vlc.h>

#include <string>
#include <vector>
#include <deque>
#include <utility>

#include "ImageProvider.h"

namespace orka {

typedef uint32_t vlc_int;

class VLCMovieProvider: public ImageProvider {
Q_OBJECT

 public:
    explicit VLCMovieProvider(const std::string & filename);
    virtual ~VLCMovieProvider();
    virtual std::pair<int, int> getFramerange();
    virtual std::string getColorSpace() {
        return "GammaCorrected";
    }
    void display(void *id);
    void lock(void ** p_pixels);
    void unlock(void *id, void * const *p_pixels);
    void setDuration(int new_duration);

 private slots:
    void start();
    void stop();
    void toggleStartStop();
    void jog(int dframes);
    void gotoFrame(int frame);

 private:
    vlc_int * frameData_;
    OrkaImage * currentFrame_;

    std::string filename_;
    bool playing_;
    unsigned int width_, height_, channels_;
    float fps_;
    libvlc_time_t max_time_ms_;

    // VLC pointers
    libvlc_instance_t * vlc_instance_;
    libvlc_media_player_t * vlc_media_player_;
    libvlc_media_t * vlc_media_;
};

} /* namespace orka */
#endif  // SRC_VLCMOVIEPROVIDER_H_
