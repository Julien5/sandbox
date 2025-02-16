use std::env::args;
use std::error::Error;
use std::num::NonZeroUsize;

use stream_download::http::HttpStream;
use stream_download::http::reqwest::Client;
use stream_download::source::{DecodeError, SourceStream};
use stream_download::storage::bounded::BoundedStorageProvider;
use stream_download::storage::memory::MemoryStorageProvider;
use stream_download::{Settings, StreamDownload};

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error + Send + Sync>> {
    let url = args().nth(1).unwrap_or_else(|| {
        "http://www.hyperion-records.co.uk/audiotest/14 Clementi Piano Sonata in D major, Op 25 No \
         6 - Movement 2 Un poco andante.MP3"
            .to_string()
    });

    let stream = HttpStream::<Client>::create(url.parse()?).await?;

    println!("content length={:?}", stream.content_length());
    println!("content type={:?}", stream.content_type());

    let reader =
        match StreamDownload::from_stream(stream, BoundedStorageProvider::new(
            // you can use any other kind of storage provider here
            MemoryStorageProvider,
            // be liberal with the buffer size, you need to make sure it holds enough space to
            // prevent any out-of-bounds reads
            NonZeroUsize::new(64 * 1024).unwrap(),
        ), Settings::default())
            .await
        {
            Ok(reader) => reader,
            Err(e) => return Err(e.decode_error().await)?,
        };


    let handle = tokio::task::spawn_blocking(move || {
        let (_stream, handle) = rodio::OutputStream::try_default()?;
        let sink = rodio::Sink::try_new(&handle)?;
        sink.append(rodio::Decoder::new(reader)?);
        sink.sleep_until_end();

        Ok::<_, Box<dyn Error + Send + Sync>>(())
    });
    handle.await??;
    Ok(())
}
