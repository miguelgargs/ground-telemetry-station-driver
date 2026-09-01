mod checksum;

fn main() {
    let byte_string = b"123456789";
    let crc = checksum::crc_calculate(byte_string, byte_string.len() as u16); // without casting it is 'usize'
    // expected `u8`, found `&[u8; 9]`
    // so when i use the 'b' in front of the string, it gets casted to a bytearray
    println!("0x{:04X}", crc);
    // i like the rust compiler, it behaves pretty good, errors are very informative as well
}
