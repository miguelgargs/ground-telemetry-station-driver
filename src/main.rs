// mod checksum;

use std::io::prelude::*;
use std::net::TcpStream;
mod check_mavlink;

fn main() {
    // rust is nice:)
    println!("Starting the program...");
    let uri = String::from("127.0.0.1");
    let port = 5760;
    let mut endpoint = String::from("");
    endpoint.push_str(&uri);
    endpoint.push_str(":");
    endpoint.push_str(&port.to_string());
    println!("[INFO] Endpoint is: {endpoint}");
    let mut stream = TcpStream::connect(&endpoint).expect("Could not create socket");
    println!("Socket is open on {endpoint}");
    let mut buf: [u8; 560] = [0; 560]; // buffer that fits two mavlink messages (max length is 280B)
    loop {
        println!("Waiting for data...");
        let read_bytes = stream.read(&mut buf); // buffer is overwritten every time a new message arrives

        println!("Received {read_bytes:?} bytes from TcpSocket!!"); // :? is for pretty-printing :)
        //println!("Buffer contents: {buf:?}");
        if read_bytes.unwrap() == 0 {
            continue; // skip this iteration if we did not get bytes
        }
        let is_mav = check_mavlink::is_mavlink(&buf);
        println!("Is MAVLink V2: {is_mav:?}");
    }
}
