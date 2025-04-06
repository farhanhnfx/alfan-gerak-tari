#ifndef ALFAN_TANGAN_H
#define ALFAN_TANGAN_H

class Tangan {
    private:
    /*
     * Sama seperti bacaGerak()
    */
    void processMotionFrame(int gerakCounter, float speed);

    public:
    /*
     * @brief Membaca file GRK<counter>.txt kemudian memproses dan menyimpannya ke ServoManager::goal_positions dan moving_speeds
     * @param gerakCounter GRK<counter>.txt berapa yang akan dibaca
     * @param speed Kecepatan, misal 5 maka diharapkan servo dapat mencapai tujuan dalam 500 milidetik
    */
    void bacaGerak(int gerakCounter, float speed);
};

#endif