# system-developer-task
Сборка проекта:
mkdir build && cd build
cmake ..
make
Проект до конца не закончен - не разобрался с shared_memory в библиотеке boost, хотя делал все по документации.
Пример запуска демонов: ./demons --mem 100 log_mem.txt ./demons --file 100 log_mem.txt
Пример подгрузки библиотеки с приложением: LD_PRELOAD=./build/libpreload_lib.so ./your_program
Библиотека перехватывает стандартные функции libc. Отправляет бинаризированные логи в общий буфер, который реализован в shared_buffer.
Демоны открывают общий буфер и принимают из очереди буфера логи и записывают в файл, название которого было задано при запуске.

