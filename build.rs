fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_prost_build::compile_protos("src/proto/connection.proto")?;
    tonic_prost_build::compile_protos("src/proto/datastream_service/datastream.proto")?;
    Ok(())
}
