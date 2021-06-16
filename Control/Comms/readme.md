Communication between ESP32 and Arduino Nano Every

This is done through UART (Universal Asynchronous Receiver/Transmitter)

UART has parameters that need to be the same on both devices to have proper communication:

-baud rate (number of bits per second that are transmitted/received)
-data length (number of bits per byte of data)
-parity bit (the parity bit is a bit added to the transmitted data and tells the receiver if the number of 1’s in the data transmitted is odd or even. The possible setting for Parity Bit is Odd or Even (1 or 0))
-number of stop bits (can use none, one or two stop bits at end of a set of bits)
-flow control (flow Control is the method to avoid the risk of losing data when transmitting data over UART. The UART device uses special characters as flow control to start/stop transmission)

UART interface on nano is the TX and RX pin next to USB port and on ESP32 is user defined I have chose UART2, which corresponds to pins 8 (RXD2) and 9 (TXD2).

To establish UART connection Serial.begin(baud_rate) has to be set in setup section (user can set baud rate). This sets up the stream to print from the received data for both ESP32 and nano. Serial is used as this is the UART port for ESP32 and nano that connects to the USB port that goes to your laptop.

For ESP32 we have to set Serial2.begin(baud_rate, config, RX_pin, TX_pin), where config is the used to change settings for data length (I used SERIAL_8N1 as it is default) and RX and TX pins can either be selected with a pin number or with a name as long as it is defined above (e.g. #define RXD2 16, which is what I did). Serial2.begin configures the stream that sends info to TX pin and identifies when info can be read from the RX pin. Serial2 here is because this is the second UART port excluding the USB to laptop.

For nano this can be configured simply with Serial1.begin(baud_rate). Serial1 is used to identify that we are sending and receiving from the TX and RX pins. Serial1 is used here as it is the other UART port that maps to the pins while Serial maps USB to laptop.

In our case Serial2.begin for ESP32 has to have same baud rate as Serial1.begin for nano.

Note: Use Serial1 for any operations in nano code and Serial2 for ESP32 code apart from when you want to send info to view on laptop (then it is just Serial)

For sending:

Use Serial1.write(xxxxx) for nano where you write the name of a variable or value in place of xxxxx
Use Serial2.write(xxxxx) for ESP32

Serial.write() sends the info to the TX pin as binary data and so should be used for ints.

Serial.print() sends data to serial port as ASCII text and so should be used for chars.

For receiving:

First need to check if anything is present at the RX port. This is done with an if statement: e.g. if(Serial1.available()) for nano.

Next one should store incoming byte. This can be done by declaring it as an int or char and setting it equal to Serial1.read() for nano or Serial2.read() for ESP32. This is only for receiving one at a time, if we need to receive multiple chars/ints we will have to create an array to store them in and print once all are received.

e.g. int incomingByte = Serial1.read();

Finally to view this info on laptop serial monitor Serial.print(xxxxx) is used to print on serial monitor. Serial.println(xxxxx) can be used if you want to start new line after this info.

e.g. Serialprintln(incomingByte)

Examples for sending and receiving single and multiple ints and chars are organised in folders
