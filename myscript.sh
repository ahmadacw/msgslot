make clean
rm message_slot
sleep 0.2
sudo rmmod message_slot
sleep 0.5
make
sleep 1.5
echo after make
sudo insmod message_slot.ko
sleep 0.1
echo after insmod
sudo mknod message_slot c 240 0
sleep 0.1
echo after mknod
sudo chmod 7777 ./message_slot
sleep 0.5
gcc -o s -Wall -std=c11 message_sender.c
gcc -o r -Wall -std=c11 message_reader.c
