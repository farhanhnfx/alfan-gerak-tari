#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <unistd.h>

extern bool panic_mode;     // Status mode panic (1) atau tidak (0)
extern bool rescue_mode;    // Status mode rescue (1) atau tidak (0)
extern int music_state;     // Status musik apakah play (1) atau mute (0)
extern int gerak_tari_current_index; // Tracker indeks gerak tari ke-berapa
extern int data_gerak_tari_others_index;    // Tracker data dari robot satunya
extern int count_time; // variabel untuk membantu sinkronisasi gerakan

class Communication {
    private:

    void starting_condition();
    bool can_communicate_with_others();
    int read_others_data();


    public:
    Communication();
    void request_music();
    void transmit_data(bool music_condition, int count);
    void receives_data();
};


#endif