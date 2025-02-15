use futures::stream::BoxStream as _;
use reqwest_streams::JsonStreamResponse as _;
use serde::Deserialize;

#[derive(Debug, Clone, Deserialize)]
struct MyTestStructure {
    some_test_field: String
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {

    let _stream = reqwest::get("http://localhost:8080/json-array")
        .await?
        .json_array_stream::<MyTestStructure>(1024);

    Ok(())
}
