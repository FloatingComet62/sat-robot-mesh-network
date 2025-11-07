move src\seeder unsrc
move src\seeder_machine unsrc

pio run --target upload --upload-port %1

move unsrc\seeder src
move unsrc\seeder_machine src