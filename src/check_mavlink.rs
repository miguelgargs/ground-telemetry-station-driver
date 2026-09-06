pub fn is_mavlink(msg: &[u8]) -> bool {
    // read the header of the received message to check if it's a mavlink message
    let first_byte = msg[0];
    println!("The first byte in the msg is: {first_byte:x?}");
    return first_byte == 0xFD;
}
