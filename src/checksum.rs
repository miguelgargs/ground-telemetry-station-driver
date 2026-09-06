/// Calculates the CRC16_MCRF4XX checksum over a byte buffer.
///
/// # Arguments
///
/// * `buffer` - The bytes to hash.
/// * `length` - Length of the byte array.
///
/// # Returns
/// * `u16` calculated checksum
/// The checksum over the buffer bytes
pub fn crc_calculate(buffer: &[u8], length: u16) -> u16 {
    let crc_tmp: u16;
    // initialize crc_tmp to empty
    crc_tmp = 0xffff;

    // while length > 0 {
    //     crc_accumulate(buffer++, &crc_tmp);
    // }

    // println!("{}", crc_tmp);
    crc_tmp
}
