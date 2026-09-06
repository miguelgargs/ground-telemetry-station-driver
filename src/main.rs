// mod checksum;

use std::io::prelude::*;
use std::net::TcpStream;
mod check_mavlink;

fn main() {
    // rust is nice:)
    println!("Starting the program...");
    let mut stream = TcpStream::connect("127.0.0.1:5760").expect("Could not create socket");
    println!("Socket is open on 127.0.0.1:5760");
    let mut buf: [u8; 560] = [0; 560]; // buffer that fits two mavlink messages (max length is 280B)
    loop {
        println!("Waiting for data...");
        let read_bytes = stream.read(&mut buf);

        println!("Received {read_bytes:?} bytes from TcpSocket!!"); // :? is for pretty-printing :)
        //println!("Buffer contents: {buf:?}");
        if read_bytes.unwrap() == 0 {
            continue; // skip this iteration if we did not get bytes
        }
        let is_mav = check_mavlink::is_mavlink(&buf);
        println!("Is MAVLink V2: {is_mav:?}");
    }
}
