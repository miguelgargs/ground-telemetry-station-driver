pub fn is_mavlink(msg: &[u8]) -> bool {
    // read the header of the received message to check if it's a mavlink message
    let first_byte = msg[0];
    println!("The first byte in the msg is: {first_byte:x?}");
    // return first_byte == 0xFD;
    match first_byte {
        0xFD => return true,
        0xFE => return true,
        _ => false,
    }
}

#[cfg(test)]
mod tests {
    use super::*; // required to see the functions in the file

    #[test]
    fn test_check_right() {
        // Check valid MAVLink header byte
        let mut buf: [u8; 2] = [0; 2];
        buf[0] = 0xFD;
        assert_eq!(is_mavlink(&buf), true);
    }

    #[test]
    fn test_check_wrong() {
        let mut buf: [u8; 2] = [0, 2];
        buf[0] = 0x00;
        assert_eq!(is_mavlink(&buf), false);
    }

    #[test]
    fn test_check_wrong_2() {
        let mut buf: [u8; 2] = [0, 2];
        buf[0] = 0x12;
        assert_eq!(is_mavlink(&buf), false);
    }
}
