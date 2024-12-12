
#define scl 19
#define sda 21
#define dev_add 0xA0

class eprom {
public:
  void write(unsigned int add, byte data) {
    i2c_start();
    i2c_write(dev_add);
    i2c_write(add);
    i2c_write(data);
    i2c_stop();
    delay(10);
  }
  uint8_t read(unsigned int add) {
    uint8_t data = 0xFF;
    i2c_start();
    i2c_write(dev_add);
    i2c_write(add);
    i2c_stop();


    i2c_start();
    i2c_write(dev_add | 1);
    data = i2c_read(0);
    i2c_stop();
    // delay(10);
    return data;
  }
  void writeString(uint8_t add, String val) {
    uint8_t dataSize = val.length();
    write(add, dataSize);
    for (uint8_t i = 0; i < dataSize; ++i) {
      write(add + 1 + i, val[i]);
    }
  }

  String readString(uint8_t add) {
    uint8_t dataSize = read(add);
    char data[dataSize + 1];
    for (uint8_t i = 0; i < dataSize; ++i) {
      data[i] = read(add + 1 + i);
    }
    data[dataSize] = '\0';
    return String(data);
  }

  void init() {
    pinMode(sda, OUTPUT);
    pinMode(scl, OUTPUT);
  }
private:

// Define timing constants
#define DELAY 5

  // Function to generate a delay
  void i2c_delay() {
    delayMicroseconds(DELAY);
  }

  // Function to set SDA as input
  void sda_input() {
    pinMode(sda, INPUT_PULLUP);
  }

  // Function to set SDA as output
  void sda_output() {
    pinMode(sda, OUTPUT);
  }

  // Function to write SDA
  void sda_write(uint8_t state) {
    digitalWrite(sda, state);
  }

  // Function to write SCL
  void scl_write(uint8_t state) {
    digitalWrite(scl, state);
  }

  // Function to start I2C communication
  void i2c_start() {
    sda_write(HIGH);
    scl_write(HIGH);
    i2c_delay();
    sda_write(LOW);
    i2c_delay();
    scl_write(LOW);
  }

  // Function to stop I2C communication
  void i2c_stop() {
    sda_write(LOW);
    scl_write(HIGH);
    i2c_delay();
    sda_write(HIGH);
    i2c_delay();
  }

  // Function to write a byte over I2C
  bool i2c_write(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
      sda_write((byte & 0x80) != 0);
      byte <<= 1;
      i2c_delay();
      scl_write(HIGH);
      i2c_delay();
      scl_write(LOW);
      i2c_delay();
    }
    sda_input();
    scl_write(HIGH);
    i2c_delay();
    bool ack = !digitalRead(sda);
    scl_write(LOW);
    sda_output();
    return ack;
  }

  // Function to read a byte over I2C
  uint8_t i2c_read(bool ack) {
    uint8_t byte = 0;
    sda_input();
    for (uint8_t i = 0; i < 8; i++) {
      scl_write(HIGH);
      i2c_delay();
      byte = (byte << 1) | digitalRead(sda);
      scl_write(LOW);
      i2c_delay();
    }
    sda_output();
    sda_write(!ack);
    scl_write(HIGH);
    i2c_delay();
    scl_write(LOW);
    sda_write(HIGH);
    return byte;
  }

} ep;
