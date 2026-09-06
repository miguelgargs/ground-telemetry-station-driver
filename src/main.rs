// mod checksum;

use std::io::prelude::*;
use std::net::TcpStream;

fn main() {
    let _byte_string = b"123456789";
    // let crc = checksum::crc_calculate(byte_string, byte_string.len() as u16); // without casting it is 'usize'
    // expected `u8`, found `&[u8; 9]`
    // so when i use the 'b' in front of the string, it gets casted to a bytearray
    // println!("0x{:04X}", crc);
    // i like the rust compiler, it behaves pretty good, errors are very informative as well
    println!("Starting the program...");
    let mut stream = TcpStream::connect("127.0.0.1:5760").expect("Could not create socket");
    println!("Socket is open on 127.0.0.1:5760");
    let mut buf = [0; 560]; // buffer that fits two mavlink messages (max length is 280B)
    loop {
        println!("Waiting for data...");
        let read_bytes = stream.read(&mut buf);

        println!("Received {read_bytes:?} bytes from TcpSocket!!"); // :? is for pretty-printing :)
        println!("Buffer contents: {buf:?}");
    }
}
