## Program Gerak Tari - Alfan GMRT UGM
### Latest Update: 6 April 2025
Package ini hingga saat ini berisikan program dua buah program, yaitu: Program Rekam Gerak dan Program Gerak Tangan (On-Development).

Link terkait: https://docs.google.com/document/d/14YH6WwX3MLx6o_tqQHlnr0oFchm8Jjc4bFIMNI2iFy8/edit?tab=t.0


<b>Langkah instalasi:</b>
1. Clone repository ini ke dalam <i>directory <b>src</b></i> pada workspace-mu! (jalankan terminal '<b><i>git clone --recurse-submodules [link git repository]</i></b>' di dalam <i>{nama_workspace}/src/</i>). Menggunakan tambahan <b><i>'--recurse-submodules'</i></b> karena di-dalam program ini juga menggunakan RapidYAML dari repository lain (https://github.com/biojppm/rapidyaml). Apabila terlanjur clone biasa tanpa --recurse-submodules maka dapat menjalankan <b><i>'git submodule update --init --recursive'</i></b> untuk mendapatkan RapidYAML.
2. Jika berhasil, maka akan muncul folder baru di dalam <i>{workspace}/src/</i> yaitu program ini sendiri.
3. Buka terminal lalu kembali ke <i>root</i> dari <i>workspace</i>-mu (bisa menggunakan perintah '<i><b>cd ..</b></i>' sehingga terminal akan berada di <i>root workspace</i>
4. Build package ini dengan menjalankan perintah '<i><b>colcon build --packages-select alfan_gerak_tari</b></i>'
5. Jika sudah, jalankan perintah '<i><b>source install/setup.bash</b></i>'

Apabila package ini sudah berhasil di-<i>build</i>, maka terdapat dua program sementara yang dapat digunakan, yaitu:
1. Program Rekam Gerak
   - Program ini dapat digunakan dengan menjalankan perintah '<i><b>ros2 run alfan_gerak_tari rekam_gerak</b></i>'
   - Apabila inisialiasi servo MX-28 dan XL-320 berhasil, maka dapat menggunakan tombol 'R' untuk melakukan rekam gerak dan 'Q' untuk keluar dari program
   - <b>Update Q1 2025:</b>
      -    Sebelum proses rekam gerak dimulai, kini akan diminta untuk menamai tariannya (kemudian akan disimpan di folder dengan nama yang sama)
      -    'B' untuk break kemudian membuat sequence gerakan baru
      -    'C' untuk cancel atau menghapus rekaman terakhir
2. Program Gerak Tari <i>(On-Development)</i>
   - Masih bersifat <i>prototyping</i> untuk migrasi dari program lama ke baru menggunakan <i>library <b>Dynamixel Workbench</b></i>
   - <i>Prototype</i> bisa digunakan dengan menjalankan perintah '<i><b>ros2 run alfan_gerak_tari gerak_tari</b></i>'
   - Karena masih bersifat <i>prototype</i>, apabila menemukan hal yang berbeda atau mungkin keliru dari migrasi program lama-baru bisa didiskusikan lagi
   - <b>Update Q1 2025:</b>
      - <b>Standalone Program:</b> Perintah <i><b>'ros2 run alfan_gerak_tari gerak_tari {argument_nama_tari}</b></i> kini menerima <i>argument</i> yaitu nama tarian yang akan dijalankan. Konfigurasi tarian yang akan dijalankan berada pada folder <b>'/config_tari/'</b>. Misal ingin menjalankan konfigurasi <i>fortune_cookie.yaml</i> maka dapat menggunakan perintah <i><b>'ros2 run alfan_gerak_tari gerak_tari fortune_cookie</b></i>
      - <b>Full Program:</b> Jika dalam keadaan <b>"deploy"</b> alias program ini dijalankan melalui <b><i>launch file</i></b>, maka konfigurasi tarian yang akan dijalankan yaitu antara <b>VAR_KIRI.yaml atau VAR_KANAN.yaml</b> bergantung pada tombol yang dipencet pada robot
