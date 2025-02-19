use std::env::args;
use std::env;
use std::error::Error;
use std::num::NonZeroUsize;

use stream_download::http::HttpStream;
use stream_download::http::reqwest::Client;
use stream_download::source::{DecodeError, SourceStream};
use stream_download::storage::bounded::BoundedStorageProvider;
use stream_download::storage::memory::MemoryStorageProvider;
use stream_download::{Settings, StreamDownload};

use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;
use std::io::Result;

// trait Decodable: Read + Seek {}

struct Reader<R : Read + Seek> {
	reader: R,
}

impl<R : Read + Seek> Reader<R> {
	fn new(orig:R) -> Self {
		Self { reader:orig }
	}
}

impl<R : Read + Seek> Read for Reader<R> {
    fn read(&mut self, buf: &mut [u8]) -> std::io::Result<usize> {
		let ret=self.reader.read(buf);
		match &ret {
			Ok(size) => println!("read={:?} bytes", size),
			Err(ex) => println!("read error: {:?}", ex)
		}
		ret
	}
}

impl<R : Read + Seek> Seek for Reader<R> {
    fn seek(&mut self, relative_position: SeekFrom) -> std::io::Result<u64> {
		let ret=self.reader.seek(relative_position);
		match &ret {
			Ok(size) => println!("seek={:?} bytes", size),
			Err(ex) => println!("seek error: {:?}", ex)
		}
		ret
	}
}

#[tokio::main]
async fn main() -> core::result::Result<(), Box<dyn Error + Send + Sync>> {
	let stdargs: Vec<String> = env::args().collect();
    let url = match stdargs.len() {
		2 => stdargs[1].clone(),
		_ => "http://direct.francemusique.fr/live/fip-midfi.mp3".to_string(),
	};

    let stream = HttpStream::<Client>::create(url.parse()?).await?;

    println!("content length={:?}", stream.content_length());
    println!("content type={:?}", stream.content_type());

    let reader0 =
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

	let reader = Reader::new(reader0);

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
