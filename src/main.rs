mod checksum;

fn main() {
    let byte_string = "123456789";
    let crc = checksum::crc_calculate(byte_string, byte_string.chars().count());
    println!("0x{:04X}", crc);
}
