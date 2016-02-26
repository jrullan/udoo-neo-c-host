# udoo-neo-c-host
The Udoo-Neo Host program is a multi-threaded C program that's run in the Linux side of the Udoo-Neo and shall provide a bridge to services and applications running in Linux to be accessed by the M4 Arduino side of the Neo. This project is inspired by Udoo-Library project by David Smerkous's [Udoo Library](https://github.com/smerkousdavid/Udoo-Library).

The goals of this project are:

1. Udoo Neo specific (David's library did not work correctly on Neo)
2. Use C programming language
2. Multi-threaded
3. Low footprint
4. Memory efficient code
5. Fast code

While the project's intention is to develop a platform of services to the Arduino side, the current scope of the project is to provide the following services to the Arduino:

* Log - Implement a file log of timestamped messages
* Debug - Implement a "terminal-like" interface to display messages sent by the Arduino
* Email - Send an email with the address, subject and message provided by the Arduino. Also, be able to add attachments to email.
* Shell - Invoke a shell command on the Linux side
* Database - Store values on persistent storage (database)
* Webcam - Access the webcam for taking a picture
