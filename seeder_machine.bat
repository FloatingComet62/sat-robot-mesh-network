move src\nonseeder unsrc
move src\seeder unsrc

pio run --target upload --upload-port %1

move unsrc\nonseeder src
move unsrc\seeder src