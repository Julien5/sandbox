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

use std::time::Duration;
use std::time::{SystemTime,UNIX_EPOCH};

// trait Decodable: Read + Seek {}

struct Reader<R : Read + Seek> {
	reader: R,
}

impl<R : Read + Seek> Reader<R> {
	fn new(orig:R) -> Self {
		Self { reader:orig }
	}
}

fn time() {
	let a = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_secs();
	println!("time:{:?}", a);
}

impl<R : Read + Seek> Read for Reader<R> {
    fn read(&mut self, buf: &mut [u8]) -> std::io::Result<usize> {
		let ret=self.reader.read(buf);
		match &ret {
			Ok(size) => {
				time();
				println!("read={:?} bytes",size);
			},		
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

struct Source<S : rodio::Source> where <S as Iterator>::Item: rodio::Sample {
	source: S,
}

impl<S : rodio::Source> Source<S> where <S as Iterator>::Item: rodio::Sample {
	fn new(orig:S) -> Self {
		Self { source:orig }
	}
}

impl<S : rodio::Source> rodio::Source for Source<S> where <S as Iterator>::Item: rodio::Sample {
	fn current_frame_len(&self) -> Option<usize> {
		let ret=self.source.current_frame_len();
		match &ret {
			Some(size) => {
				time();
				println!("current frame length={:?} bytes", size);
			},
			None => println!("no length")
		}
		ret
	}
	fn channels(&self) -> u16 {
		let ret=self.source.channels();
		println!("{:?} channels", ret);
		ret
	}
	fn sample_rate(&self) -> u32 {
		let ret=self.source.sample_rate();
		println!("{:?} Hz", ret);
		ret
	}
	fn total_duration(&self) -> Option<Duration> {
		let ret=self.source.total_duration();
		match &ret {
			Some(size) => println!("total duration={:?} seconds", size),
			None => println!("no duration")
		}
		ret
	}
}

impl<S : rodio::Source> Iterator for Source<S> where <S as Iterator>::Item: rodio::Sample {
	type Item = S::Item;
	fn next(&mut self) -> Option<<Self as Iterator>::Item> {
		let ret=self.source.next();
		match &ret {
			Some(_) => println!("next"),
			None => println!("no next"),
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
            NonZeroUsize::new(128 * 1024).unwrap(),
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
		let source = Source::new(rodio::Decoder::new(reader)?);
        sink.append(source);
        sink.sleep_until_end();

        Ok::<_, Box<dyn Error + Send + Sync>>(())
    });
    handle.await??;
    Ok(())
}
