# system-developer-task
#### Сборка проекта:
 mkdir build && cd build
- cmake ..
- make
#### Пример использования:
- Пример запуска демонов: ./demons --mem 100 log_mem.txt ./demons --file 100 log_mem.txt
- Пример подгрузки библиотеки с приложением: LD_PRELOAD=./build/libpreload_lib.so ./your_program
#### Как работает
- Библиотека перехватывает стандартные функции libc. Отправляет бинаризированные логи в общий буфер, который реализован в shared_buffer. Библиотека создает общий буфер через конструктор.
- Демоны открывают общий буфер и принимают из очереди буфера логи и записывают в файл, название которого было задано при запуске. Демоны открывают общий буфер через конструктор.
- Проект до конца не закончен - не разобрался с shared_memory в библиотеке boost, хотя делал все по документации.

