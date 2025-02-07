## Program Tangan - Alfan GMRT UGM
Package ini hingga saat ini berisikan program dua buah program, yaitu: Program Rekam Gerak dan Program Gerak Tangan (On-Development).

Link terkait: https://docs.google.com/document/d/14YH6WwX3MLx6o_tqQHlnr0oFchm8Jjc4bFIMNI2iFy8/edit?tab=t.0


<b>Langkah instalasi:</b>
1. Clone repository ini ke dalam <i>directory <b>src</b></i> pada workspace-mu! (jalankan terminal '<b><i>git clone [link git repository]</i></b>' di dalam <i>{nama_workspace}/src/</i>)
2. Jika berhasil, maka akan muncul folder baru di dalam <i>{workspace}/src/</i> yaitu program ini sendiri.
3. Buka terminal lalu kembali ke <i>root</i> dari <i>workspace</i>-mu (bisa menggunakan perintah '<i><b>cd ..</b></i>' sehingga terminal akan berada di <i>root workspace</i>
4. Build package ini dengan menjalankan perintah '<i><b>colcon build --packages-select program_tangan_alfan</b></i>'
5. Jika sudah, jalankan perintah '<i><b>source install/setup.bash</b></i>'

Apabila package ini sudah berhasil di-<i>build</i>, maka terdapat dua program sementara yang dapat digunakan, yaitu:
1. Program Rekam Gerak
   - Program ini dapat digunakan dengan menjalankan perintah '<i><b>ros2 run program_tangan_alfan rekam_gerak</b></i>'
   - Apabila inisialiasi servo MX-28 dan XL-320 berhasil, maka dapat menggunakan tombol 'R' untuk melakukan rekam gerak dan 'Q' untuk keluar dari program
2. Program Gerak Tangan <i>(On-Development)</i>
   - Masih bersifat <i>prototyping</i> untuk migrasi dari program lama ke baru menggunakan <i>library <b>Dynamixel Workbench</b></i>
   - <i>Prototype</i> bisa digunakan dengan menjalankan perintah '<i><b>ros2 run program_tangan_alfan tangan_driver</b></i>'
   - Karena masih bersifat <i>prototype</i>, apabila menemukan hal yang berbeda atau mungkin keliru dari migrasi program lama-baru bisa didiskusikan lagi
