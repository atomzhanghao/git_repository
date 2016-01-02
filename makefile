DIR_METER = ./reade_write_meter
OBJ0 = modbus.o modbus-rtu.o  modbus-tcp.o modbus-data.o master_read_10sec.o
OBJ1 = modbus.o modbus-rtu.o  modbus-tcp.o modbus-data.o master_read_60sec.o
OBJ2 = modbus.o modbus-rtu.o  modbus-tcp.o modbus-data.o master_read_10min.o
OBJ3 = modbus.o modbus-rtu.o  modbus-tcp.o modbus-data.o modbus_init_meter.o
exefile = master_read_10sec master_read_60sec master_read_10min modbus_init_meter

all: $(exefile)
master_read_10sec: $(OBJ0)
	$(CC) $(CFLAGS) $(OBJ0) -o $(DIR_METER)/master_read_10sec

master_read_60sec: $(OBJ1)
	$(CC)	$(CFLAGS) $(OBJ1) -o $(DIR_METER)/master_read_60sec

master_read_10min: $(OBJ2)
	$(CC)$(CFLAGS) $(OBJ2) -o $(DIR_METER)/master_read_10min
modbus_init_meter: $(OBJ3)
	$(CC)$(CFLAGS) $(OBJ3) -o $(DIR_METER)/modbus_init_meter
modbus.o: modbus.c
	$(CC) $(CFLAGS) -c modbus.c

mobus-tcp.o: modbus-tcp.c
	$(CC) $(CFLAGS) -c modbus-tcp.c

modbus-rtu.o: modbus-rtu.c
	$(CC) $(CFLAGS) -c modbus-rtu.c

modbus-data.o: modbus-data.c
	$(CC) $(CFLAGS) -c modbus-data.c

master_read_10sec.o: $(DIR_METER)/master_read_10sec.c
	$(CC) $(CFLAGS) -c  $(DIR_METER)/master_read_10sec.c

master_read_60sec.o: $(DIR_METER)/master_read_60sec.c
	$(CC) $(CFLAGS) -c $(DIR_METER)/master_read_60sec.c

master_read_10min.o: $(DIR_METER)/master_read_10min.c
	$(CC) $(CFLAGS) -c $(DIR_METER)/master_read_10min.c

modbus_init_meter.o: $(DIR_METER)/modbus_init_meter.c
	$(CC) $(CFLAGS) -c $(DIR_METER)/modbus_init_meter.c

.PHONY: clean

clean :
	-rm $(OBJ0) $(exefile)  master_read_60sec.o master_read_10min.o modbus_init_meter.o


