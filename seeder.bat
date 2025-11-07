move src\nonseeder unsrc
move src\seeder_machine unsrc

pio run --target upload --upload-port %1

move unsrc\nonseeder src
move unsrc\seeder_machine src